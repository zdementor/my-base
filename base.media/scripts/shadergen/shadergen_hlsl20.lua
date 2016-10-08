
function HLSL20GenVertexShader(vtype, pass, perpixel, lightcnt, uniforms)

	local text = ""
	local components = vid.getVertexComponents(vtype)
	
	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(vtype, pass, perpixel, lightcnt, uniforms)
	text = text.."struct VS_INPUT\n"
	text = text.."{\n"
	text = text.."    VEC4 Vertex : POSITION;\n"
	if bit.band(components, vid.EVC_NRM) ~= 0 then
		text = text.."    VEC3 Normal : NORMAL;\n"
	end
	if bit.band(components, vid.EVC_COL) ~= 0 then
		text = text.."    VEC4 Color : COLOR0;\n"
	end
	local tci = 0
	if bit.band(components, vid.EVC_TC0) ~= 0 then
		text = text.."    VEC2 MultiTexCoord0 : TEXCOORD0;\n"
		tci = tci + 1
	end
	if bit.band(components, vid.EVC_TC1) ~= 0 then
		text = text..string.format("    VEC2 MultiTexCoord1 : TEXCOORD%d;\n", tci)
		tci = tci + 1
	end
	if bit.band(components, vid.EVC_TBN) ~= 0 then
		text = text..string.format("    VEC3 MultiTexCoord2 : TEXCOORD%d;\n", tci)
		text = text..string.format("    VEC3 MultiTexCoord3 : TEXCOORD%d;\n", tci + 1)
		tci = tci + 2
	end
	text = text.."};\n\n"
	text = text.."struct VS_OUTPUT\n"
	text = text.."{\n"
	text = text.."VARY VEC4 PositionMVP : POSITION;\n"
	text = text..AppendVarying(vtype, pass, perpixel, lightcnt, uniforms)
	text = text.."};\n\n"
	text = text.."VS_OUTPUT main(VS_INPUT input)\n"
	text = text.."{\n"
	text = text.."    VS_OUTPUT output;\n\n"
	text = text..AppendVertShaderBody(vtype, pass, perpixel, lightcnt)
	text = text.."    output.PositionMVP  = positionMVP;\n"
	text = text.."    return output;\n"	
	text = text.."}\n"
	
	return text
end
	
function HLSL20GenPixelShader(vtype, pass, perpixel, lightcnt, uniforms)

	local text = ""

	text = text..AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)
	text = text..AppendUniforms(vtype, pass, perpixel, lightcnt, uniforms)
	text = text.."struct PS_INPUT\n"
	text = text.."{\n"
	text = text..AppendVarying(vtype, pass, perpixel, lightcnt, uniforms)
	text = text.."};\n\n"
	text = text.."struct PS_OUTPUT\n"
	text = text.."{\n"
	text = text.."    VEC4 FragData0 : COLOR0;\n"
	text = text.."};\n\n"	
	text = text.."PS_OUTPUT main(PS_INPUT input)\n"
	text = text.."{\n"
	text = text.."    PS_OUTPUT output;\n\n"
	text = text..AppendPixelShaderBody(vtype, pass, perpixel, lightcnt)
	text = text.."    return output;\n"	
	text = text.."}\n"

	return text.."\n"
end
