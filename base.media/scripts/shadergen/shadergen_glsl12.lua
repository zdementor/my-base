
function GLSL12GenVertexShader(vtype, pass, perpixel, lightcnt, uniforms)

	local text = "#version 120\n\n"

	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendAttributes()
	text = text.."\n"
	text = text..AppendVarying(vtype, pass, perpixel, lightcnt, uniforms)
	text = text.."\n"
	text = text.."void main(void)\n"
	text = text.."{\n"
	text = text..AppendVertShaderBody(vtype, pass, perpixel, lightcnt)	
	text = text.."    gl_Position = positionMVP;\n"	
	text = text.."}\n"

	return text
end
	
function GLSL12GenPixelShader(vtype, pass, perpixel, lightcnt, uniforms)

	local text = "#version 120\n\n"

	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendVarying(vtype, pass, perpixel, lightcnt, uniforms)
	text = text.."\n"
	text = text.."void main(void)\n"
	text = text.."{\n"
	text = text..AppendPixelShaderBody(vtype, pass, perpixel, lightcnt)
	text = text.."}\n"

	return text
end
