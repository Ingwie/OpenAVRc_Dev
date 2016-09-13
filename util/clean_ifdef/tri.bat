for /r %%a in ("*.cpp") do sunifdef --replace -xc --line --undef CPUARM %%a
for /r %%a in ("*.h") do sunifdef --replace -xc --line --undef CPUARM %%a
for /r %%a in ("*.txt") do sunifdef --replace -xc --line --undef CPUARM %%a

for /r %%a in ("*.cpp") do sunifdef --replace -xc --line --undef PCBTARANIS %%a
for /r %%a in ("*.h") do sunifdef --replace -xc --line --undef PCBTARANIS %%a
for /r %%a in ("*.txt") do sunifdef --replace -xc --line --undef PCBTARANIS %%a

for /r %%a in ("*.cpp") do sunifdef --replace -xc --line --undef PCBSKY9X %%a
for /r %%a in ("*.h") do sunifdef --replace -xc --line --undef PCBSKY9X %%a
for /r %%a in ("*.txt") do sunifdef --replace -xc --line --undef PCBSKY9X %%a

for /r %%a in ("*.cpp") do sunifdef --replace -xc --line --undef COLORLCD %%a
for /r %%a in ("*.h") do sunifdef --replace -xc --line --undef COLORLCD %%a
for /r %%a in ("*.txt") do sunifdef --replace -xc --line --undef COLORLCD %%a

for /r %%a in ("*.cpp") do sunifdef --replace -xc --line --undef LUA %%a
for /r %%a in ("*.h") do sunifdef --replace -xc --line --undef LUA %%a
for /r %%a in ("*.txt") do sunifdef --replace -xc --line --undef LUA %%a

pause
