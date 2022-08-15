-- =============================================================================
-- ROOT_PATH/executables/SOUND/MIDI_in
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")