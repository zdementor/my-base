
function GLSL12GenVertexShader(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)

	local text = "#version 120\n\n"

	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(uniforms, lightcnt)
	text = text..AppendAttributes(attribs)
	text = text.."\n"
	text = text..AppendVaryings(varyings)
	text = text.."\n"
	text = text.."void main(void)\n"
	text = text.."{\n"
	text = text..AppendVertShaderBody(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)
	text = text.."    gl_Position = positionMVP;\n"	
	text = text.."}\n"

	return text
end
	
function GLSL12GenPixelShader(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)

	local text = "#version 120\n\n"

	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(uniforms, lightcnt)
	text = text..AppendVaryings(varyings)
	text = text.."\n"
	text = text.."void main(void)\n"
	text = text.."{\n"
	text = text..AppendPixelShaderBody(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)
	text = text.."}\n"

	return text
end
