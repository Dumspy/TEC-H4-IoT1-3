# Agent Guidelines for ESP32 PlatformIO Project

## Build/Test Commands
- Build: `platformio run` or `pio run`
- Upload to device: `platformio run --target upload` or `pio run -t upload`
- Clean build: `platformio run --target clean` or `pio run -t clean`
- Monitor serial: `platformio device monitor` or `pio device monitor`
- Run tests: `platformio test` or `pio test`

## Project Structure
- Source files: `src/` (Arduino C++ framework)
- Headers: `include/`
- Tests: `test/`
- Config: `platformio.ini` (ESP32 DOIT DevKit v1 board)

## Code Style
- Language: C++ (Arduino framework)
- Naming: `UPPER_SNAKE_CASE` for constants, `camelCase` for variables/functions
- Comments: Use `//` for single-line comments explaining hardware connections and logic
- Pin definitions: Define hardware pins as named constants at top of file
- Serial debugging: Use `Serial.begin(9600)` in setup, `Serial.print/println` for output
- Types: Use Arduino types (`int`, `bool`, `unsigned long`) and explicit pin numbers
- Error handling: Not applicable for embedded Arduino code (no exceptions)
- Formatting: 2-space indentation, braces on same line
