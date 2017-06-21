#!/usr/bin/env bash

TIMER='/usr/bin/time --quiet -f [%E]'

List() {
    ./$1 -l src.tar.1.mz src.tar.5.mz src.tar.9.mz
}

Check() {
    cmp src.tar src.tar.1
    cmp src.tar src.tar.5
    cmp src.tar src.tar.9
}

rm -rf src.tar*
echo -n 'Cleaning up...                                         ' ; $TIMER make clean > /dev/null
echo -n 'Building MaZIP (UNIX)...                               ' ; $TIMER make mazip > /dev/null
echo -n 'Building MaZIP (WINDOWS)...                            ' ; $TIMER make mazip.exe > /dev/null
echo -n 'Creating src.tar...                                    ' ; $TIMER tar -cf src.tar src

echo -n 'Fast   compressing src.tar > src.tar.1.mz (UNIX)...    ' ; $TIMER ./mazip -1c src.tar > src.tar.1.mz
echo -n 'Normal compressing src.tar > src.tar.5.mz (UNIX)...    ' ; $TIMER ./mazip -c src.tar > src.tar.5.mz
echo -n 'Best   compressing src.tar > src.tar.5.mz (UNIX)...    ' ; $TIMER ./mazip -9c src.tar > src.tar.9.mz
echo    'Results:' ; List mazip
echo -n 'Decompressing src.tar.*.mz > src.tar.* (UNIX)...       ' ; $TIMER ./mazip -d src.tar.1.mz src.tar.5.mz src.tar.9.mz
echo    'Checking (UNIX)...' ; Check

echo -n 'Fast   compressing src.tar > src.tar.1.mz (WINDOWS)... ' ; $TIMER ./mazip.exe -1c src.tar > src.tar.1.mz
echo -n 'Normal compressing src.tar > src.tar.5.mz (WINDOWS)... ' ; $TIMER ./mazip.exe -c src.tar > src.tar.5.mz
echo -n 'Best   compressing src.tar > src.tar.5.mz (WINDOWS)... ' ; $TIMER ./mazip.exe -9c src.tar > src.tar.9.mz
echo    'Results:' ; List mazip.exe
echo -n 'Decompressing src.tar.*.mz > src.tar.* (WINDOWS)...    ' ; $TIMER ./mazip.exe -d src.tar.1.mz src.tar.5.mz src.tar.9.mz
echo    'Checking (WINDOWS)... ' ; Check
echo    'Done testing'
