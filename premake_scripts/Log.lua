require 'Object'

Log = Object:new(
{
	indent = 0
})

function Log:IncrementIndent()
	self.indent = self.indent + 1
end

function Log:DecrementIndent()
	self.indent = self.indent - 1
end

function Log:GetIndentSpaces()
	local txt = ""
	for i = 1, self.indent do
		txt = "  " .. txt
	end
	return txt
end

function Log:Output(txt)
	local ident_txt = self:GetIndentSpaces()
	local lines = string.explode(txt, "\n")
	for k, line in pairs(lines) do
		print(ident_txt .. line)
	end
end