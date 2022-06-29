require 'Object'

--------------------------------------------------------------------
-- Class declaration
--------------------------------------------------------------------
Log = Object:new(
{
	indent = 0
})

--------------------------------------------------------------------
-- increment the indent level
--------------------------------------------------------------------
function Log:IncrementIndent()
	self.indent = self.indent + 1
end

--------------------------------------------------------------------
-- decrement the indent level
--------------------------------------------------------------------
function Log:DecrementIndent()
	self.indent = self.indent - 1
end

--------------------------------------------------------------------
-- get the string corresponding to the spaces required at the start of the lines
--------------------------------------------------------------------
function Log:GetIndentSpaces()
	local txt = ""
	for i = 1, self.indent do
		txt = "  " .. txt
	end
	return txt
end

--------------------------------------------------------------------
-- display a string with spaces corresponding to indent level
--------------------------------------------------------------------
function Log:Output(txt)
	local ident_txt = self:GetIndentSpaces()
	local lines = string.explode(txt, "\n")
	for k, line in pairs(lines) do
		print(ident_txt .. line)
	end
end