
function HLSL20GenVertexShader(info, pass)

	local text = ""
	
	text = text..AppendDefines(info, true)
	text = text..AppendUniforms(info, true)
	text = text.."struct VS_INPUT\n"
	text = text.."{\n"
	text = text..AppendAttributes(info)
	text = text.."};\n\n"
	text = text.."struct VS_OUTPUT\n"
	text = text.."{\n"
	text = text.."VARY VEC4 PositionMVP : POSITION;\n"
	text = text..AppendVaryings(info)
	text = text.."};\n\n"
	text = text.."VS_OUTPUT main(VS_INPUT input)\n"
	text = text.."{\n"
	text = text.."    VS_OUTPUT output;\n\n"
	text = text..AppendVertShaderBody(info, pass)
	text = text.."    output.PositionMVP = "..Tokens.PositionMVP..";\n"
	text = text.."    return output;\n"	
	text = text.."}\n"
	
	return text
end
	
function HLSL20GenPixelShader(info, pass)

	local text = ""

	text = text..AppendDefines(info, false)
	text = text..AppendUniforms(info, false)
	text = text.."struct PS_INPUT\n"
	text = text.."{\n"
	text = text..AppendVaryings(info)
	text = text.."};\n\n"
	text = text.."struct PS_OUTPUT\n"
	text = text.."{\n"
	text = text.."VEC4 FragData0 : COLOR0;\n"
	if info.isDS then
		text = text.."VEC4 FragData1 : COLOR1;\n"
		text = text.."VEC4 FragData2 : COLOR2;\n"
		text = text.."VEC4 FragData3 : COLOR3;\n"
	end
	text = text.."};\n\n"	
	text = text.."PS_OUTPUT main(PS_INPUT input)\n"
	text = text.."{\n"
	text = text.."    PS_OUTPUT output;\n\n"
	text = text..AppendPixelShaderBody(info, pass)
	text = text.."    return output;\n"	
	text = text.."}\n"

	return text.."\n"
end
