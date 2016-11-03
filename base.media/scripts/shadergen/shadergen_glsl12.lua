
function GLSL12GenVertexShader(info, pass)

	local text = "#version 120\n\n"

	text = text..AppendDefines(info, true)
	text = text..AppendUniforms(info, true)
	text = text..AppendAttributes(info)
	text = text.."\n"
	text = text..AppendVaryings(info)
	text = text.."\n"
	text = text.."void main(void)\n"
	text = text.."{\n"
	text = text..AppendVertShaderBody(info, pass)
	text = text.."    gl_Position = positionMVP;\n"	
	text = text.."}\n"

	return text
end
	
function GLSL12GenPixelShader(info, pass)

	local text = "#version 120\n\n"

	text = text..AppendDefines(info, false)
	text = text..AppendUniforms(info, false)
	text = text..AppendVaryings(info)
	text = text.."\n"
	text = text.."void main(void)\n"
	text = text.."{\n"
	text = text..AppendPixelShaderBody(info, pass)
	text = text.."}\n"

	return text
end
