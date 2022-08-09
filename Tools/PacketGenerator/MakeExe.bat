pushed %~dp0
pyinstaller --onefile PacketGenerator.py
MOVE .\dist\PacketGenerator.exe .\GenPackets.exe
@RD /S /Q .\build
@RD /s /Q .\dist
DEL /S /F /Q .\PacketGenerator.spec
PAUSE