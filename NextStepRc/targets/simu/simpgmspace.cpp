/*
 *************************************************************
 *                      NEXTSTEPRC                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

#include "../../nextsteprc.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#if defined(RTCLOCK)
  #include <time.h>
#endif

#if defined WIN32 || !defined __GNUC__
  #include <direct.h>
#endif

#if defined(SIMU_DISKIO)
  FILE * diskImage = 0;
#endif


volatile uint8_t pina=0xff, pinb=0xff, pinc=0xff, pind, pine=0xff, pinf=0xff, ping=0xff, pinh=0xff, pinj=0, pinl=0;
uint8_t portb, portc, porth=0, dummyport;
uint16_t dummyport16;
const char *eepromFile = NULL;
FILE *fp = NULL;
int g_snapshot_idx = 0;

#if defined(EEPROM_RLC)
  extern uint16_t eeprom_pointer;
  extern uint8_t * eeprom_buffer_data;
#else
  uint32_t eeprom_pointer;
  uint8_t * eeprom_buffer_data;
  volatile int32_t eeprom_buffer_size;
  bool eeprom_read_operation;
  #define EESIZE_SIMU (128*4096) // TODO why here?
#endif

#if !defined(EESIZE_SIMU)
  #define EESIZE_SIMU EESIZE
#endif

#if defined(SDCARD) && !defined(SKIP_FATFS_DECLARATION)
char simuSdDirectory[1024] = "";
#endif

uint8_t eeprom[EESIZE_SIMU];
sem_t *eeprom_write_sem;

void simuInit()
{/*
  for (int i = 0; i <= 17; i++) {
    simuSetSwitch(i, 0);
    simuSetKey(i, false);  // a little dirty, but setting keys that don't exist is perfectly OK here
  }*/
}

#define NEG_CASE(sw_or_key, pin, mask) \
    case sw_or_key: \
      if ((int)state > 0) pin &= ~(mask); else pin |= (mask); \
      break;
#define POS_CASE(sw_or_key, pin, mask) \
    case sw_or_key: \
      if ((int)state > 0) pin |= (mask); else pin &= ~(mask); \
      break;

  #if defined(PCBMEGA2560)
    #define SWITCH_CASE POS_CASE
  #else
    #define SWITCH_CASE NEG_CASE
  #endif
  #define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if ((int)state >= 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if ((int)state <= 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;
  #define KEY_CASE POS_CASE
  #define TRIM_CASE KEY_CASE

void simuSetKey(uint8_t key, bool state)
{
  // TRACE("simuSetKey(%d, %d)", key, state);
  switch (key) {
    KEY_CASE(KEY_MENU, KEYS_GPIO_REG_MENU, KEYS_GPIO_PIN_MENU)
    KEY_CASE(KEY_EXIT, KEYS_GPIO_REG_EXIT, KEYS_GPIO_PIN_EXIT)
    KEY_CASE(KEY_RIGHT, KEYS_GPIO_REG_RIGHT, KEYS_GPIO_PIN_RIGHT)
    KEY_CASE(KEY_LEFT, KEYS_GPIO_REG_LEFT, KEYS_GPIO_PIN_LEFT)
    KEY_CASE(KEY_UP, KEYS_GPIO_REG_UP, KEYS_GPIO_PIN_UP)
    KEY_CASE(KEY_DOWN, KEYS_GPIO_REG_DOWN, KEYS_GPIO_PIN_DOWN)
#if   defined(PCBGRUVIN9X) || defined(PCBMEGA2560)
    KEY_CASE(BTN_REa, pind, 0x20)
#elif defined(ROTARY_ENCODER_NAVIGATION)
    KEY_CASE(BTN_REa, RotEncoder, 0x20)
#endif
  }
}

void simuSetTrim(uint8_t trim, bool state)
{
  // TRACE("trim=%d state=%d", trim, state);

  switch (trim) {
    TRIM_CASE(0, TRIMS_GPIO_REG_LHL, TRIMS_GPIO_PIN_LHL)
    TRIM_CASE(1, TRIMS_GPIO_REG_LHR, TRIMS_GPIO_PIN_LHR)
    TRIM_CASE(2, TRIMS_GPIO_REG_LVD, TRIMS_GPIO_PIN_LVD)
    TRIM_CASE(3, TRIMS_GPIO_REG_LVU, TRIMS_GPIO_PIN_LVU)
    TRIM_CASE(4, TRIMS_GPIO_REG_RVD, TRIMS_GPIO_PIN_RVD)
    TRIM_CASE(5, TRIMS_GPIO_REG_RVU, TRIMS_GPIO_PIN_RVU)
    TRIM_CASE(6, TRIMS_GPIO_REG_RHL, TRIMS_GPIO_PIN_RHL)
    TRIM_CASE(7, TRIMS_GPIO_REG_RHR, TRIMS_GPIO_PIN_RHR)
  }
}

// TODO use a better numbering to allow google tests to work on Taranis
void simuSetSwitch(uint8_t swtch, int8_t state)
{
  // TRACE("simuSetSwitch(%d, %d)", swtch, state);
  switch (swtch) {
#if   defined(PCBGRUVIN9X)
    SWITCH_CASE(0, ping, 1<<INP_G_ThrCt)
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pinc, 1<<INP_C_ElevDR)
    SWITCH_3_CASE(3, ping, pinb, (1<<INP_G_ID1), (1<<INP_B_ID2))
    SWITCH_CASE(4, pinc, 1<<INP_C_AileDR)
    SWITCH_CASE(5, ping, 1<<INP_G_Gear)
    SWITCH_CASE(6, pinb, 1<<INP_B_Trainer)
#elif defined(PCBMEGA2560)
    SWITCH_CASE(0, ping, 1<<INP_G_ThrCt)
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pinc, 1<<INP_L_ElevDR)
    SWITCH_3_CASE(3, pinc, pinc, (1<<INP_C_ID1), (1<<INP_C_ID2))
    SWITCH_CASE(4, pinc, 1<<INP_C_AileDR)
    SWITCH_CASE(5, ping, 1<<INP_G_Gear)
    SWITCH_CASE(6, pinb, 1<<INP_L_Trainer)
#else // PCB9X
#if defined(JETI) || defined(FRSKY) || defined(NMEA) || defined(ARDUPILOT)
    SWITCH_CASE(0, pinc, 1<<INP_C_ThrCt)
    SWITCH_CASE(4, pinc, 1<<INP_C_AileDR)
#else
    SWITCH_CASE(0, pine, 1<<INP_E_ThrCt)
    SWITCH_CASE(4, pine, 1<<INP_E_AileDR)
#endif
    SWITCH_3_CASE(3, ping, pine, (1<<INP_G_ID1), (1<<INP_E_ID2))
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pine, 1<<INP_E_ElevDR)
    SWITCH_CASE(5, pine, 1<<INP_E_Gear)
    SWITCH_CASE(6, pine, 1<<INP_E_Trainer)
#endif

    default:
      break;
  }
}

uint16_t getTmr16KHz()
{
  return get_tmr10ms() * 160;
}

bool eeprom_thread_running = true;
void *eeprom_write_function(void *)
{/*
  while (!sem_wait(eeprom_write_sem)) {
    if (!eeprom_thread_running)
      return NULL;
    if (fp) {
      if (fseek(fp, eeprom_pointer, SEEK_SET) == -1)
        perror("error in fseek");
    }
    while (--eeprom_buffer_size) {
      assert(eeprom_buffer_size > 0);
      if (fp) {
        if (fwrite(eeprom_buffer_data, 1, 1, fp) != 1)
          perror("error in fwrite");
        sleep(5);
      }
      else {
        memcpy(&eeprom[eeprom_pointer], eeprom_buffer_data, 1);
      }
      eeprom_pointer++;
      eeprom_buffer_data++;

      if (fp && eeprom_buffer_size == 1) {
        fflush(fp);
      }
    }
  }
  return 0;*/
}

uint8_t main_thread_running = 0;
char * main_thread_error = NULL;
extern void nextsteprcStart();
void *main_thread(void *)
{
#ifdef SIMU_EXCEPTIONS
  signal(SIGFPE, sig);
  signal(SIGSEGV, sig);

  try {
#endif


    s_current_protocol[0] = 255;

    menuLevel = 0;
    menuHandlers[0] = menuMainView;
    menuHandlers[1] = menuModelSelect;

    eeReadAll(); // load general setup and selected model

#if defined(SIMU_DISKIO)
    f_mount(&g_FATFS_Obj, "", 1);
    // call sdGetFreeSectors() now because f_getfree() takes a long time first time it's called
    sdGetFreeSectors();
#endif


    if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

    if (main_thread_running == 1) {
      nextsteprcStart();
    }
    else {
    }

    s_current_protocol[0] = 0;

    while (main_thread_running) {
      perMain();
      //sleep(10);
    }


#ifdef SIMU_EXCEPTIONS
  }
  catch (...) {
    main_thread_running = 0;
  }
#endif

#if defined(SIMU_DISKIO)
  if (diskImage) {
    fclose(diskImage);
  }
#endif

  return NULL;
}

#if defined WIN32 || !defined __GNUC__
#define chdir  _chdir
#define getcwd _getcwd
#endif

/*
pthread_t main_thread_pid;
void StartMainThread(bool tests)
{
#if defined(SDCARD)
  if (strlen(simuSdDirectory) == 0)
    getcwd(simuSdDirectory, 1024);
#endif


  //
    g_tmr10ms must be non-zero otherwise some SF functions (that use this timer as a marker when it was last executed)
    will be executed twice on startup. Normal radio does not see this issue because g_tmr10ms is already a big number
    before the first call to the Special Functions. Not so in the simulator.

    There is another issue, some other function static variables depend on this value. If simulator is started
    multiple times in one Companion session, they are set to their initial values only first time the simulator
    is started. Therefore g_tmr10ms must also be set to non-zero value only the first time, then it must be left
    alone to continue from the previous simulator session value. See the issue #2446

  */
  if (g_tmr10ms == 0) {
    g_tmr10ms = 1;
  }

#if defined(RTCLOCK)
  g_rtcTime = time(0);
#endif

  main_thread_running = (tests ? 1 : 2);
  pthread_create(&main_thread_pid, NULL, &main_thread, NULL);
}
*/
void StopMainThread()
{
  main_thread_running = 0;
  pthread_join(main_thread_pid, NULL);
}



pthread_t eeprom_thread_pid;

void StartEepromThread(const char *filename)
{
  eepromFile = filename;
  if (eepromFile) {
    fp = fopen(eepromFile, "rb+");
    if (!fp)
      fp = fopen(eepromFile, "wb+");
    if (!fp) perror("error in fopen");
  }
#ifdef __APPLE__
  eeprom_write_sem = sem_open("eepromsem", O_CREAT, S_IRUSR | S_IWUSR, 0);
#else
  eeprom_write_sem = (sem_t *)malloc(sizeof(sem_t));
  sem_init(eeprom_write_sem, 0, 0);
#endif

  eeprom_thread_running = true;
  assert(!pthread_create(&eeprom_thread_pid, NULL, &eeprom_write_function, NULL));
}

void StopEepromThread()
{
  eeprom_thread_running = false;
  sem_post(eeprom_write_sem);
  pthread_join(eeprom_thread_pid, NULL);
#ifdef __APPLE__
  //TODO free semaphore eeprom_write_sem
#else
  sem_destroy(eeprom_write_sem);
  free(eeprom_write_sem);
#endif

  if (fp) fclose(fp);
}



uint16_t stackAvailable()
{
  return 500;
}

#if 0
static void EeFsDump(){
  for(int i=0; i<EESIZE; i++)
  {
    printf("%02x ",eeprom[i]);
    if(i%16 == 15) puts("");
  }
  puts("");
}
#endif

#if defined(SDCARD) && !defined(SKIP_FATFS_DECLARATION) && !defined(SIMU_DISKIO)
namespace simu {
//#include <dirent.h>
#if !defined WIN32
  #include <libgen.h>
#endif
}
#include "../../thirdparty/FatFs/ff.h"

#if defined WIN32 || !defined __GNUC__
#include <direct.h>
#include <stdlib.h>
#endif

#include <map>
#include <string>


//char *convertSimuPath(const char *path)
{
  static char result[1024];
  if (((path[0] == '/') || (path[0] == '\\')) && (strcmp(simuSdDirectory, "/") != 0))
    sprintf(result, "%s%s", simuSdDirectory, path);
  else
    strcpy(result, path);

  return result;
}

typedef std::map<std::string, std::string> filemap_t;

filemap_t fileMap;

/*char *findTrueFileName(const char *path)
{
  TRACE("findTrueFileName(%s)", path);
  static char result[1024];
  filemap_t::iterator i = fileMap.find(path);
  if (i != fileMap.end()) {
    strcpy(result, i->second.c_str());
    TRACE("\tfound in map: %s", result);
    return result;
  }
  else {
    //find file
    //add to map
#if defined WIN32 || !defined __GNUC__
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath(path, drive, dir, fname, ext);
    std::string fileName = std::string(fname) + std::string(ext);
    std::string dirName = std::string(drive) + std::string(dir);
    std::string searchName = dirName + "*";
    // TRACE("\tsearching for: %s", fileName.c_str());
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(searchName.c_str(), &ffd);
    if (INVALID_HANDLE_VALUE != hFind) {
      do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          //TRACE("comparing with: %s", ffd.cFileName);
          if (!strcasecmp(fileName.c_str(), ffd.cFileName)) {
            strcpy(result, dirName.c_str());
            strcat(result, ffd.cFileName);
            TRACE("\tfound: %s", ffd.cFileName);
            fileMap.insert(filemap_t:: value_type(path, result));
            return result;
          }
        }
      }
      while (FindNextFile(hFind, &ffd) != 0);
    }
#else
    strcpy(result, path);
    std::string fileName = simu::basename(result);
    strcpy(result, path);
    std::string dirName = simu::dirname(result);
    simu::DIR * dir = simu::opendir(dirName.c_str());
    if (dir) {
      // TRACE("\tsearching for: %s", fileName.c_str());
      for (;;) {
        struct simu::dirent * res = simu::readdir(dir);
        if (res == 0) break;
#if defined(__APPLE__)
        if ((res->d_type == DT_REG) || (res->d_type == DT_LNK)) {
#else
        if ((res->d_type == simu::DT_REG) || (res->d_type == simu::DT_LNK)) {
#endif
          // TRACE("comparing with: %s", res->d_name);
          if (!strcasecmp(fileName.c_str(), res->d_name)) {
            strcpy(result, dirName.c_str());
            strcat(result, "/");
            strcat(result, res->d_name);
            TRACE("\tfound: %s", res->d_name);
            fileMap.insert(filemap_t:: value_type(path, result));
            simu::closedir(dir);
            return result;
          }
        }
      }
      simu::closedir(dir);
    }
#endif
  }
  TRACE("\tnot found");
  strcpy(result, path);
  return result;
}

FRESULT f_stat (const TCHAR * name, FILINFO *)
{
  char *path = convertSimuPath(name);
  char * realPath = findTrueFileName(path);
  struct stat tmp;
  if (stat(realPath, &tmp)) {
    TRACE("f_stat(%s) = error %d (%s)", path, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  else {
    TRACE("f_stat(%s) = OK", path);
    return FR_OK;
  }
}

FRESULT f_mount (FATFS* ,const TCHAR*, BYTE opt)
{
  return FR_OK;
}

FRESULT f_open (FIL * fil, const TCHAR *name, BYTE flag)
{
  char *path = convertSimuPath(name);
  char * realPath = findTrueFileName(path);
  if (!(flag & FA_WRITE)) {
    struct stat tmp;
    if (stat(realPath, &tmp)) {
      TRACE("f_open(%s) = INVALID_NAME", path);
      return FR_INVALID_NAME;
    }
    fil->fsize = tmp.st_size;
    fil->fptr = 0;
  }
  fil->fs = (FATFS*)fopen(realPath, (flag & FA_WRITE) ? ((flag & FA_CREATE_ALWAYS) ? "wb+" : "ab+") : "rb+");
  fil->fptr = 0;
  if (fil->fs) {
    TRACE("f_open(%s, %x) = %p (FIL %p)", path, flag, fil->fs, fil);
    return FR_OK;
  }
  TRACE("f_open(%s) = error %d (%s)", path, errno, strerror(errno));
  return FR_INVALID_NAME;
}

FRESULT f_read (FIL* fil, void* data, UINT size, UINT* read)
{
  if (fil && fil->fs) {
    *read = fread(data, 1, size, (FILE*)fil->fs);
    fil->fptr += *read;
    // TRACE("fread(%p) %u, %u", fil->fs, size, *read);
  }
  return FR_OK;
}

FRESULT f_write (FIL* fil, const void* data, UINT size, UINT* written)
{
  if (fil && fil->fs) {
    *written = fwrite(data, 1, size, (FILE*)fil->fs);
    fil->fptr += size;
    // TRACE("fwrite(%p) %u, %u", fil->fs, size, *written);
  }
  return FR_OK;
}

FRESULT f_lseek (FIL* fil, DWORD offset)
{
  if (fil && fil->fs) fseek((FILE*)fil->fs, offset, SEEK_SET);
  fil->fptr = offset;
  return FR_OK;
}

UINT f_size(FIL* fil)
{
  if (fil && fil->fs) {
    long curr = ftell((FILE*)fil->fs);
    fseek((FILE*)fil->fs, 0, SEEK_END);
    long size = ftell((FILE*)fil->fs);
    fseek((FILE*)fil->fs, curr, SEEK_SET);
    TRACE("f_size(%p) %u", fil->fs, size);
    return size;
  }
  return 0;
}

FRESULT f_close (FIL * fil)
{
  assert(fil);
  TRACE("f_close(%p) (FIL:%p)", fil->fs, fil);
  if (fil->fs) {
    fclose((FILE*)fil->fs);
    fil->fs = NULL;
  }
  return FR_OK;
}

FRESULT f_chdir (const TCHAR *name)
{
  chdir(convertSimuPath(name));
  return FR_OK;
}

FRESULT f_opendir (DIR * rep, const TCHAR * name)
{
  char *path = convertSimuPath(name);
  rep->fs = (FATFS *)simu::opendir(path);
  if ( rep->fs ) {
    TRACE("f_opendir(%s) = OK", path);
    return FR_OK;
  }
  TRACE("f_opendir(%s) = error %d (%s)", path, errno, strerror(errno));
  return FR_NO_PATH;
}

FRESULT f_closedir (DIR * rep)
{
  TRACE("f_closedir(%p)", rep);
  simu::closedir((simu::DIR *)rep->fs);
  return FR_OK;
}

FRESULT f_readdir (DIR * rep, FILINFO * fil)
{
  if (!rep->fs) return FR_NO_FILE;
  simu::dirent * ent = simu::readdir((simu::DIR *)rep->fs);
  if (!ent) return FR_NO_FILE;

#if defined(WIN32) || !defined(__GNUC__) || defined(__APPLE__)
  fil->fattrib = (ent->d_type == DT_DIR ? AM_DIR : 0);
#else
  if (ent->d_type == simu::DT_UNKNOWN) {
    fil->fattrib = 0;
    struct stat buf;
    if (stat(ent->d_name, &buf) == 0) {
      fil->fattrib = (S_ISDIR(buf.st_mode) ? AM_DIR : 0);
    }
  }
  else {
    fil->fattrib = (ent->d_type == simu::DT_DIR ? AM_DIR : 0);
  }
#endif

  memset(fil->fname, 0, 13);
  memset(fil->lfname, 0, SD_SCREEN_FILE_LENGTH);
  strncpy(fil->fname, ent->d_name, 13-1);
  strcpy(fil->lfname, ent->d_name);
  // TRACE("f_readdir(): %s", fil->fname);
  return FR_OK;
}

FRESULT f_mkfs (const TCHAR *path, BYTE, UINT)
{
  TRACE("Format SD...");
  return FR_OK;
}

FRESULT f_mkdir (const TCHAR*)
{
  return FR_OK;
}

FRESULT f_unlink (const TCHAR* name)
{
  char *path = convertSimuPath(name);
  if (unlink(path)) {
    TRACE("f_unlink(%s) = error %d (%s)", path, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  TRACE("f_unlink(%s) = OK", path);
  return FR_OK;
}

FRESULT f_rename(const TCHAR *oldname, const TCHAR *newname)
{
  if (rename(oldname, newname) < 0) {
    TRACE("f_rename(%s, %s) = error %d (%s)", oldname, newname, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  TRACE("f_rename(%s, %s) = OK", oldname, newname);
  return FR_OK;
}

int f_putc (TCHAR c, FIL * fil)
{
  if (fil && fil->fs) fwrite(&c, 1, 1, (FILE*)fil->fs);
  return FR_OK;
}

int f_puts (const TCHAR * str, FIL * fil)
{
  int n;
  for (n = 0; *str; str++, n++) {
    if (f_putc(*str, fil) == EOF) return EOF;
  }
  return n;
}

int f_printf (FIL *fil, const TCHAR * format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  if (fil && fil->fs) vfprintf((FILE*)fil->fs, format, arglist);
  va_end(arglist);
  return 0;
}

FRESULT f_getcwd (TCHAR *path, UINT sz_path)
{
  char cwd[1024];
  if (!getcwd(cwd, 1024)) {
    TRACE("f_getcwd() = getcwd() error %d (%s)", errno, strerror(errno));
    strcpy(path, ".");
    return FR_NO_PATH;
  }

  if (strlen(cwd) < strlen(simuSdDirectory)) {
    TRACE("f_getcwd() = logic error strlen(cwd) < strlen(simuSdDirectory):  cwd: \"%s\",  simuSdDirectory: \"%s\"", cwd, simuSdDirectory);
    strcpy(path, ".");
    return FR_NO_PATH;
  }

  // remove simuSdDirectory from the cwd
  strcpy(path, cwd + strlen(simuSdDirectory));

  TRACE("f_getcwd() = %s", path);
  return FR_OK;
}

FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs)
{
  // just fake that we always have some clusters free
  *nclst = 10;
  return FR_OK;
}


#endif  // #if defined(SDCARD) && !defined(SKIP_FATFS_DECLARATION) && !defined(SIMU_DISKIO)


#if defined(SIMU_DISKIO)
#include "FatFs/diskio.h"
#include <time.h>
#include <stdio.h>


int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj) /// Create a sync object
{
  return 1;
}

int ff_req_grant (_SYNC_t sobj)        // Lock sync object
{
  return 1;
}

void ff_rel_grant (_SYNC_t sobj)        // Unlock sync object
{

}

int ff_del_syncobj (_SYNC_t sobj)        // Delete a sync object
{
  return 1;
}

DWORD get_fattime (void)
{
  time_t tim = time(0);
  const struct tm * t = gmtime(&tim);

  // Pack date and time into a DWORD variable
  return ((DWORD)(t->tm_year - 80) << 25)
    | ((uint32_t)(t->tm_mon+1) << 21)
    | ((uint32_t)t->tm_mday << 16)
    | ((uint32_t)t->tm_hour << 11)
    | ((uint32_t)t->tm_min << 5)
    | ((uint32_t)t->tm_sec >> 1);
}

unsigned int noDiskStatus = 0;

void traceDiskStatus()
{
  if (noDiskStatus > 0) {
    TRACE("disk_status() called %d times", noDiskStatus);
    noDiskStatus = 0;
  }
}

DSTATUS disk_initialize (BYTE pdrv)
{
  traceDiskStatus();
  TRACE("disk_initialize(%u)", pdrv);
  diskImage = fopen("sdcard.image", "r+");
  return diskImage ? (DSTATUS)0 : (DSTATUS)STA_NODISK;
}

DSTATUS disk_status (BYTE pdrv)
{
  ++noDiskStatus;
  // TRACE("disk_status(%u)", pdrv);
  return (DSTATUS)0;
}

DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
  if (diskImage == 0) return RES_NOTRDY;
  traceDiskStatus();
  TRACE("disk_read(%u, %p, %u, %u)", pdrv, buff, sector, count);
  fseek(diskImage, sector*512, SEEK_SET);
  fread(buff, count, 512, diskImage);
  return RES_OK;
}

DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
  if (diskImage == 0) return RES_NOTRDY;
  traceDiskStatus();
  TRACE("disk_write(%u, %p, %u, %u)", pdrv, buff, sector, count);
  fseek(diskImage, sector*512, SEEK_SET);
  fwrite(buff, count, 512, diskImage);
  return RES_OK;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
  if (diskImage == 0) return RES_NOTRDY;
  traceDiskStatus();
  TRACE("disk_ioctl(%u, %u, %p)", pdrv, cmd, buff);
  if (pdrv) return RES_PARERR;

  DRESULT res;
  BYTE *ptr = (BYTE *)buff;

  if (cmd == CTRL_POWER) {
    switch (*ptr) {
      case 0:         // Sub control code == 0 (POWER_OFF)
        res = RES_OK;
        break;
      case 1:         // Sub control code == 1 (POWER_ON)
        res = RES_OK;
        break;
      case 2:         // Sub control code == 2 (POWER_GET)
        *(ptr+1) = (BYTE)1;  // fake powered
        res = RES_OK;
        break;
      default :
        res = RES_PARERR;
    }
    return res;
  }

  switch(cmd) {
// Generic command (Used by FatFs)
    case CTRL_SYNC :     // Complete pending write process (needed at _FS_READONLY == 0)
      break;

    case GET_SECTOR_COUNT: // Get media size (needed at _USE_MKFS == 1)
      {
        struct stat buf;
        if (stat("sdcard.image", &buf) == 0) {
          DWORD noSectors  = buf.st_size / 512;
          *(DWORD*)buff = noSectors;
          TRACE("disk_ioctl(GET_SECTOR_COUNT) = %u", noSectors);
          return RES_OK;
        }
        return RES_ERROR;
      }

    case GET_SECTOR_SIZE: // Get sector size (needed at _MAX_SS != _MIN_SS)
      TRACE("disk_ioctl(GET_SECTOR_SIZE) = 512");
      *(WORD*)buff = 512;
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE : // Get erase block size (needed at _USE_MKFS == 1)
      *(WORD*)buff = 512 * 4;
      res = RES_OK;
      break;

    case CTRL_TRIM : // Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1)
      break;

// Generic command (Not used by FatFs)
    case CTRL_LOCK : // Lock/Unlock media removal
    case CTRL_EJECT: // Eject media
    case CTRL_FORMAT: // Create physical format on the media
      return RES_PARERR;


// MMC/SDC specific ioctl command
    // case MMC_GET_TYPE    10  // Get card type
    // case MMC_GET_CSD     11  // Get CSD
    // case MMC_GET_CID     12  // Get CID
    // case MMC_GET_OCR     13  // Get OCR
    // case MMC_GET_SDSTAT    14  // Get SD status

// ATA/CF specific ioctl command
    // case ATA_GET_REV     20  // Get F/W revision
    // case ATA_GET_MODEL   21  // Get model name
    // case ATA_GET_SN      22  // Get serial number
    default:
      return RES_PARERR;
  }
  return RES_OK;
}*/

uint32_t sdIsHC()
{
  return sdGetSize() > 2000000;
}

uint32_t sdGetSpeed()
{
  return 330000;
}

#endif // #if defined(SIMU_DISKIO)



bool lcd_refresh = true;
display_t lcd_buf[DISPLAY_BUF_SIZE];

void lcdSetRefVolt(uint8_t val)
{
}

void adcPrepareBandgap()
{
}


void lcdRefresh()
{
  memcpy(lcd_buf, displayBuf, sizeof(lcd_buf));
  lcd_refresh = true;
}

void telemetryPortInit()
{
}


