
function HLSL20GenVertexShader(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)

	local text = ""
	local components = vid.getVertexComponents(vtype)
	local aParams = ShaderGenInfo.Attribs.Params
	local aMask = ShaderGenInfo.Attribs.Mask
	
	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(uniforms, lightcnt)
	text = text.."struct VS_INPUT\n"
	text = text.."{\n"
	text = text..AppendAttributes(attribs)
	text = text.."};\n\n"
	text = text.."struct VS_OUTPUT\n"
	text = text.."{\n"
	text = text.."VARY VEC4 PositionMVP : POSITION;\n"
	text = text..AppendVaryings(varyings)
	text = text.."};\n\n"
	text = text.."VS_OUTPUT main(VS_INPUT input)\n"
	text = text.."{\n"
	text = text.."    VS_OUTPUT output;\n\n"
	text = text..AppendVertShaderBody(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)
	text = text.."    output.PositionMVP  = positionMVP;\n"
	text = text.."    return output;\n"	
	text = text.."}\n"
	
	return text
end
	
function HLSL20GenPixelShader(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)

	local text = ""

	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(uniforms, lightcnt)
	text = text.."struct PS_INPUT\n"
	text = text.."{\n"
	text = text..AppendVaryings(varyings)
	text = text.."};\n\n"
	text = text.."struct PS_OUTPUT\n"
	text = text.."{\n"
	text = text.."    VEC4 FragData0 : COLOR0;\n"
	text = text.."};\n\n"	
	text = text.."PS_OUTPUT main(PS_INPUT input)\n"
	text = text.."{\n"
	text = text.."    PS_OUTPUT output;\n\n"
	text = text..AppendPixelShaderBody(vtype, pass, perpixel, lightcnt, uniforms, attribs, varyings)
	text = text.."    return output;\n"	
	text = text.."}\n"

	return text.."\n"
end
