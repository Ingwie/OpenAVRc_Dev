path C:\OpenAVRc_Builder\Python34
for /r %%a in ("*.cpp") do CALL python copyright.py ./copyright.hd %%a
for /r %%a in ("*.c") do CALL python copyright.py ./copyright.hd %%a
for /r %%a in ("*.h") do CALL python copyright.py ./copyright.hd %%a
for /r %%a in ("*.txt") do CALL python copyright.py ./copyright.hd %%a
pause
