@echo off
echo Creating patch file...
diff -urN --exclude=.svn --exclude=debug --exclude=release --exclude=Debug --exclude=Release --exclude=*.ncb --exclude=*.suo ET_SDK 2.60 > patches\omni-bot.patch
echo done.
pause
