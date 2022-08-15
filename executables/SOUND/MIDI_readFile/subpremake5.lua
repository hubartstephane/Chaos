-- =============================================================================
-- ROOT_PATH/executables/SOUND/MIDI_readFile
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")