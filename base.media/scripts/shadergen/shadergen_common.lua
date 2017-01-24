
function FetchTexColTbl(pass, textype)
	local texColTbl = {}
	local tblIdx = 1
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		local texture = pass.Layers[i]:getTexture()
		local rgb_scale =  pass.Layers[i]:getTexRGBScale()
		if texture ~= nil and pass.Layers[i]:getType() == textype then
			local colstr = ""
			if rgb_scale == 2 then
				colstr = string.format("VEC4(tcol%d.rgb*2.0,tcol%d.a)", i, i)
			elseif rgb_scale == 3 then
				colstr = string.format("VEC4(tcol%d.rgb*4.0,tcol%d.a)", i, i)
			else
				colstr = string.format("tcol%d", i)
			end
			texColTbl[tblIdx] = colstr
			tblIdx = tblIdx + 1
		end
	end
	return texColTbl
end

function AppendDefines(info, vsh)
	local text = ""
	local uniforms = 0
	if vsh then
		uniforms = info.Uniforms.VertMask
	else
		uniforms = info.Uniforms.FragMask
	end

	if MyDriver:getDriverFamily() == vid.EDF_OPENGL then
		text = text.."#define FLOAT float\n"
		text = text.."#define INT   int\n"
		text = text.."#define MAT3  mat3\n"
		text = text.."#define MAT4  mat4\n"
		text = text.."#define VEC2  vec2\n"
		text = text.."#define VEC3  vec3\n"
		text = text.."#define VEC4  vec4\n"
		text = text.."#define SAMP2D sampler2D\n"
		text = text.."#define TEX2D texture2D\n"
		text = text.."#define VS_IN(vvv) vvv\n"
		text = text.."#define VS_OUT(vvv) ##vvv\n"
		text = text.."#define PS_IN(vvv) ##vvv\n"
		text = text.."#define PS_OUT(vvv,nnn) gl_##vvv[nnn]\n"
		if vsh then
			text = text.."#define ATTR attribute\n"
		end
		text = text.."#define VARY varying\n"
		text = text.."#define UNI uniform\n"
		text = text.."#define MIX mix\n"
		text = text.."#define MUL(v1,v2) (v2*v1)\n"
	else
		text = text.."#define FLOAT float\n"
		text = text.."#define INT   int\n"
		text = text.."#define MAT3  float3x3\n"
		text = text.."#define MAT4  float4x4\n"
		text = text.."#define VEC2  float2\n"
		text = text.."#define VEC3  float3\n"
		text = text.."#define VEC4  float4\n"
		text = text.."#define SAMP2D sampler2D\n"
		text = text.."#define TEX2D tex2D\n"
		text = text.."#define VS_IN(vvv) input.##vvv\n"
		text = text.."#define VS_OUT(vvv) output.##vvv\n"
		text = text.."#define PS_IN(vvv) input.##vvv\n"
		text = text.."#define PS_OUT(vvv,nnn) output.##vvv##nnn\n"
		if vsh then
			text = text.."#define ATTR\n"
		end
		text = text.."#define VARY\n"
		text = text.."#define UNI uniform\n"
		text = text.."#define MIX lerp\n"
		text = text.."#define MUL(v1,v2) mul(v1,v2)\n"
	end
	text = text.."\n"
	if bit.band(uniforms, vid.EUF_MATERIAL_COLORS) ~= 0 then
		text = text.."#define M_DIF(mat)  mat[0].rgba\n"
		text = text.."#define M_AMB(mat)  mat[1].rgba\n"
		text = text.."#define M_SPEC(mat) mat[2].rgba\n"
		text = text.."#define M_EMIS(mat) mat[3].rgba\n"
		text = text.."\n"
	end
	if bit.band(uniforms, vid.EUF_LIGHTING) ~= 0 then
		text = text.."#define L_DIF(lig)  lig[0].rgba\n"
		text = text.."#define L_AMB(lig)  lig[1].rgba\n"
		text = text.."#define L_SPEC(lig) lig[2].rgba\n"
		text = text.."#define L_POS(lig)  lig[3].xyz\n"
		text = text.."#define L_R(lig)    lig[3].w\n"
		text = text.."\n"
	end

	if info.hasNMap and not vsh then
		text = text.."#define PARALAX_SCALE 0.03\n"
		text = text.."#define PARALAX_BIAS -PARALAX_SCALE/2.0\n"
		text = text.."\n"
	end

	if info.fogging then
		text = text.."#define FOG_START(fog)    fog.x\n"
		text = text.."#define FOG_END(fog)      fog.y\n"
		text = text.."#define FOG_DENSITY(fog)  fog.z\n"
		text = text.."\n"
	end

	return text
end

function AppendAttributes(info)
	local text=""
	local attribs = info.Attribs
	local aParams = attribs.Params
	local aMask = attribs.Mask
	local aDecl = "ATTR "
	for atype = 0, vid.E_ATTRIB_TYPE_COUNT-1 do	
		if bit.band(aMask, vid.getAttribFlag(atype)) ~= 0 then
			text = text..aDecl..aParams[atype].Type.." "..aParams[atype].Name..aParams[atype].Spec..";\n"
		end
	end
	return text
end

function AppendUniforms(info, vsh)
	local text = ""
	local uniforms = 0
	if vsh then
		uniforms = info.Uniforms.VertMask
	else
		uniforms = info.Uniforms.FragMask
	end
	
	if bit.band(uniforms, vid.EUF_MODEL_VIEW_PROJ_MATRIX) ~= 0 then
		text = text.."UNI MAT4 "..Uniforms.ModelViewProjMatrix..";\n"
	end
	if bit.band(uniforms, vid.EUF_MODEL_VIEW_MATRIX) ~= 0 then
		text = text.."UNI MAT4 "..Uniforms.ModelViewMatrix..";\n"
	end
	if bit.band(uniforms, vid.EUF_MODEL_MATRIX) ~= 0 then
		text = text.."UNI MAT4 "..Uniforms.ModelMatrix..";\n"
	end
	if bit.band(uniforms, vid.EUF_MODEL_VIEW_MATRIX_3X3) ~= 0 then
		text = text.."UNI MAT3 "..Uniforms.ModelViewMatrix3x3..";\n"
	end
	if bit.band(uniforms, vid.EUF_MODEL_MATRIX_3X3) ~= 0 then
		text = text.."UNI MAT3 "..Uniforms.ModelMatrix3x3..";\n"
	end
	if bit.band(uniforms, vid.EUF_GLOBAL_AMBIENT_COLOR) ~= 0 then
		text = text.."UNI VEC4 "..Uniforms.GlobalAmbientColor..";\n"
	end
	if bit.band(uniforms, vid.EUF_MATERIAL_COLORS) ~= 0 then
		text = text.."UNI VEC4 "..Uniforms.MatColors.."[4];\n"
	end
	if bit.band(uniforms, vid.EUF_MATERIAL_SHININESS) ~= 0 then
		text = text.."UNI FLOAT "..Uniforms.MatShininess..";\n"
	end
	if bit.band(uniforms, vid.EUF_EYE_POS) ~= 0 then
		text = text.."UNI VEC3 "..Uniforms.EyePos..";\n"
	end
	if bit.band(uniforms, vid.EUF_LIGHTING) ~= 0 then
		if info.lightcnt == 1 then
			text = text.."UNI MAT4 "..Uniforms.Lighting..";\n"
		else
			text = text.."UNI MAT4 "..Uniforms.Lighting.."["..info.lightcnt.."];\n"
		end
	end
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		if bit.band(uniforms, TexFlags[i+1]) ~=0 then
			if info.ogl then
				text = text..string.format("UNI SAMP2D "..Uniforms.Tex[i+1]..";\n")
			else
				text = text..string.format("UNI SAMP2D "..Uniforms.Tex[i+1].." : register(s%d);\n", i)
			end
		end
		if bit.band(uniforms, TexMatrixFlags[i+1]) ~=0 then
			text = text..string.format("UNI MAT4 "..Uniforms.TexMatrix[i+1]..";\n", i)
		end
	end
	if bit.band(uniforms, vid.EUF_FOG_PARAMS) ~= 0 then
		text = text..string.format("UNI VEC3 "..Uniforms.FogParams..";\n")
	end
	if bit.band(uniforms, vid.EUF_FOG_COLOR) ~= 0 then
		text = text..string.format("UNI VEC3 "..Uniforms.FogColor..";\n")
	end
	if uniforms ~= 0 then
		text = text.."\n"
	end

	return text
end

function AppendVaryings(info)
	local text = ""
	local colIdx = 0
	local tcIdx = 0
	for i = 1, VARY_COUNT - 1 do
		local v = info.Varyings[i]
		if v ~= nil then
			local spec = ""
			if not info.ogl then
				if i == VARY_COL or i == VARY_SPEC then
					spec = string.format(" : COLOR%d", colIdx);
					colIdx = colIdx + 1
				else
					spec = string.format(" : TEXCOORD%d", tcIdx);
					tcIdx = tcIdx + 1
				end
			end
			text = text..string.format("VARY "..v.Type.." "..v.VarName..spec..";\n")
		end
	end
	return text
end

function AppendVaryingsOutVars(varyings)
	local text = ""
	for i = 1, VARY_COUNT - 1 do
		local v = varyings[i]
		if v ~= nil then
			text = text..string.format("    "..v.Type.." "..v._VarName..";\n")
		end
	end
	return text
end

function AppendVaryingsOut(varyings)
	local text = ""
	for i = 1, VARY_COUNT - 1 do
		local v = varyings[i]
		if v ~= nil then
			text = text..string.format("    VS_OUT("..v.VarName..") = "..v._VarName..";\n")
		end
	end
	return text
end

function AppendVaryingsInVars(varyings)
	local text = ""
	for i = 1, VARY_COUNT - 1 do
		local v = varyings[i]
		if v ~= nil then
			text = text..string.format("    "..v.Type.." "..v._VarName.." = PS_IN("..v.VarName..");\n")
		end
	end
	return text
end

function AppendMaterialDiffuse()
	return "    VEC4 mDif = M_DIF("..Uniforms.MatColors..");\n"
end

function AppendMaterialEmissive()
	return "    VEC4 mEmis = M_EMIS("..Uniforms.MatColors..");\n"
end

function AppendLighting(info, vsh, pass)
	local text = ""
	
	text = text..AppendMaterialDiffuse()
	text = text.."    VEC4 mAmb = M_AMB("..Uniforms.MatColors..");\n"
	text = text.."    VEC4 mSpec = M_SPEC("..Uniforms.MatColors..");\n"
	text = text..AppendMaterialEmissive()
	text = text.."\n"
	text = text.."    VEC3 specular = VEC3(0.0,0.0,0.0);\n"
	text = text.."    VEC4 color = VEC4(0.0, 0.0, 0.0, mDif.a);\n"
	text = text.."    color.rgb = "..Uniforms.GlobalAmbientColor..".rgb * mAmb.rgb + mEmis.rgb;\n"
	if pass:getLightingMode() ~= vid.ELM_NONE then
		for i = 0, info.lightcnt - 1 do
			text = text..string.format("    // lighting from Light %d\n", i)
			text = text.."    {\n"
			if info.lightcnt == 1 then
				text = text..string.format("        VEC4 lDif  = L_DIF("..Uniforms.Lighting..");\n")
				text = text..string.format("        VEC4 lAmb  = L_AMB("..Uniforms.Lighting..");\n")
				text = text..string.format("        VEC4 lSpec = L_SPEC("..Uniforms.Lighting..");\n")
				text = text..string.format("        VEC3 lPos  = L_POS("..Uniforms.Lighting..");\n")
				text = text..string.format("        FLOAT lR   = L_R("..Uniforms.Lighting..");\n")
			else
				text = text..string.format("        VEC4 lDif  = L_DIF("..Uniforms.Lighting.."[%d]);\n", i)
				text = text..string.format("        VEC4 lAmb  = L_AMB("..Uniforms.Lighting.."[%d]);\n", i)
				text = text..string.format("        VEC4 lSpec = L_SPEC("..Uniforms.Lighting.."[%d]);\n", i)
				text = text..string.format("        VEC3 lPos  = L_POS("..Uniforms.Lighting.."[%d]);\n", i)
				text = text..string.format("        FLOAT lR  = L_R("..Uniforms.Lighting.."[%d]);\n", i)
			end
			if info.perpixel and HasTBN(info.vtype) and info.hasNMap then
				if i < 3 then
					text = text.."        VEC3 lVec = normalize("..info.Varyings[VARY_LVEC0 + i]._VarName..".xyz);\n"
				else
					text = text.."        VEC3 lVec = normalize(VEC3("..
						info.Varyings[VARY_LVEC0]._VarName..".w, "..
						info.Varyings[VARY_LVEC1]._VarName..".w, "..
						info.Varyings[VARY_LVEC2]._VarName..".w));\n"
				end
				text = text.."        FLOAT lDist = length(lPos - "..Tokens.Position..".xyz);\n"
			else
				text = text.."        VEC3 lVec = lPos - "..Tokens.Position..".xyz;\n"
				text = text.."        FLOAT lDist = length(lVec);\n"
				text = text..string.format("        lVec = normalize(lVec);\n")
			end
			text = text.."        FLOAT NdotL = max(dot("..Tokens.Normal..", lVec), 0.0);\n"
			text = text.."        FLOAT idx = min(lDist / lR, 1.0);\n"
			text = text.."        FLOAT atten = -idx * idx + 1.0;\n"
			text = text.."        VEC3 reflVec = reflect(-lVec, "..Tokens.Normal..");\n"
			text = text.."        FLOAT specPow = pow(max(dot(eyeVec, reflVec),0.0), "..Uniforms.MatShininess..");\n"
			text = text.."        color.rgb += atten * (NdotL * mDif.rgb * lDif.rgb + mAmb.rgb * lAmb.rgb);\n"
			text = text.."        specular += atten * specPow * mSpec.rgb * lSpec.rgb;\n"
			text = text.."    }\n"
		end
	end
	if not vsh and info.hasAttenMap then
		text = text.."    color *= tatten;\n"
		text = text.."    specular *= tatten.rgb;\n"
	end
	text = text.."\n"
	return text
end

function AppendVertShaderBody(info, pass)
	local text = ""

	text = text..AppendVaryingsOutVars(info.Varyings)
	text = text.."\n"

	text = text.."    VEC4 vertex = VS_IN("..Attribs.Position..");\n"
	text = text.."    VEC4 "..Tokens.PositionMVP.." = MUL(vertex,"..Uniforms.ModelViewProjMatrix..");\n"
	text = text.."\n"

	local normalDefined = false

	local normMatrix = Uniforms.ModelViewMatrix3x3
	local posMatrix = Uniforms.ModelViewMatrix
	if info.isDS then
		normMatrix = Uniforms.ModelMatrix3x3
		posMatrix = Uniforms.ModelMatrix
	end

	if IsNeedNormal(info.vtype, pass, info.lightcnt) then
		text = text.."    VEC4 "..Tokens.Position.." = MUL(vertex,"..posMatrix..");\n"
		text = text.."    VEC3 eyeVec = -"..Tokens.Position..".xyz;\n"
		if info.isDS then
			text = text.."    eyeVec += "..Uniforms.EyePos..";\n"
		end
		text = text.."    VEC3 "..Tokens.Normal.." = VS_IN("..Attribs.Normal..");\n"
		text = text.."    "..Tokens.Normal.." = MUL("..Tokens.Normal..","..normMatrix..");\n\n"
		if not info.perpixel then
			text = text.."    "..Tokens.Normal.." = normalize(N);\n"
			text = text.."    eyeVec = normalize(eyeVec);\n"
		end
		normalDefined = true
	elseif info.hasNMap then
		text = text.."    VEC4 "..Tokens.Position.." = MUL(vertex,"..posMatrix..");\n"
		text = text.."    VEC3 eyeVec = -"..Tokens.Position..".xyz;\n"
		if info.isDS then
			text = text.."    eyeVec = "..Uniforms.EyePos.."- eyeVec;\n"
		end
	elseif info.Varyings[VARY_POS] ~= nil then
		text = text.."    VEC4 "..Tokens.Position.." = MUL(vertex,"..posMatrix..");\n"
	end

	if info.isDS and not normalDefined then
		if info.vnormal then
			text = text.."    VEC3 "..Tokens.Normal.." = VS_IN("..Attribs.Normal..");\n"
			text = text.."    "..Tokens.Normal.." = MUL("..Tokens.Normal..","..normMatrix..");\n\n"
		else
			text = text.."    VEC3 "..Tokens.Normal.." = VEC3(0., 1., 0.);\n"
		end
	end

		
	if info.light and info.vnormal and info.perpixel == false then
		text = text.."\n"
		text = text..AppendLighting(info, true, pass)
		text = text.."    "..info.Varyings[VARY_SPEC]._VarName.." = specular;\n"
	elseif info.mcolor then
		text = text..AppendMaterialDiffuse()
		text = text..AppendMaterialEmissive()
	end

	local normalPassed = false

	if info.vnormal and info.perpixel then
		if HasTBN(info.vtype) and info.hasNMap then
			text = text.."    VEC3 "..Tokens.Tangent.." = VS_IN("..Attribs.Tangent..").xyz;\n"
			text = text.."    VEC3 "..Tokens.Binormal.." = VS_IN("..Attribs.Binormal..").xyz;\n"
			text = text.."    "..Tokens.Tangent.."  = MUL("..Tokens.Tangent..","..normMatrix..");\n"
			text = text.."    "..Tokens.Binormal.." = MUL("..Tokens.Binormal..","..normMatrix..");\n"
			text = text.."    "..info.Varyings[VARY_EYE]._VarName..
				" = VEC3(dot(eyeVec, "..Tokens.Tangent.."), dot(eyeVec, "..Tokens.Binormal.."), dot(eyeVec, "..Tokens.Normal.."));\n"
			if pass:getLightingMode() ~= vid.ELM_NONE then
				text = text..string.format("    // transforming into TBN space\n")
				for i = 0, info.lightcnt - 1 do
					text = text..string.format("    { // Light %d\n", i)
					if info.lightcnt == 1 then
						text = text..string.format("        VEC3 lVec = L_POS("..Uniforms.Lighting..") - "..Tokens.Position..".xyz;\n")
					else
						text = text..string.format("        VEC3 lVec = L_POS("..Uniforms.Lighting.."[%d]) - "..Tokens.Position..".xyz;\n", i)
					end
					if i < 3 then
						text = text.."        "..info.Varyings[VARY_LVEC0 + i]._VarName..
							" = VEC4(dot(lVec, "..Tokens.Tangent.."), dot(lVec, "..Tokens.Binormal.."), dot(lVec, "..Tokens.Normal.."), 0.0);\n"
					else					
						text = text.."        "..info.Varyings[VARY_LVEC0]._VarName..".w = dot(lVec, "..Tokens.Tangent..");\n"
						text = text.."        "..info.Varyings[VARY_LVEC1]._VarName..".w = dot(lVec, "..Tokens.Binormal..");\n"
						text = text.."        "..info.Varyings[VARY_LVEC2]._VarName..".w = dot(lVec, "..Tokens.Normal..");\n"
					end
					text = text.."    }\n"					
				end
			end
		elseif info.light or info.hasNMap then
			text = text.."    "..info.Varyings[VARY_EYE]._VarName.." = eyeVec;\n"
		end
	end
	if info.vcolor then
		if info.light and info.vnormal and info.perpixel == false then
			text = text.."    "..info.Varyings[VARY_COL]._VarName.." = VS_IN("..Attribs.Color..") * color;\n"
		else
			text = text.."    "..info.Varyings[VARY_COL]._VarName.." = VS_IN("..Attribs.Color..");\n"
		end
	elseif info.light and info.vnormal and info.perpixel == false then
		text = text.."    "..info.Varyings[VARY_COL]._VarName.." = color;\n"
	elseif info.mcolor then
		text = text.."    "..info.Varyings[VARY_COL]._VarName.." = VEC4(mEmis.rgb, mDif.a);\n"
	end

	if info.Varyings[VARY_POS] ~= nil then
		text = text.."    "..info.Varyings[VARY_POS]._VarName.." = "..Tokens.Position..";\n"
	end
	if info.Varyings[VARY_TANG] ~= nil
			and info.Varyings[VARY_BINORM] ~= nil
			and info.Varyings[VARY_NORM] ~= nil then
		text = text.."    MAT3 tbn = MAT3("..Tokens.Tangent..","..Tokens.Binormal..","..Tokens.Normal..");\n"
		text = text.."    tbn = transpose(tbn);\n"
		text = text.."    "..info.Varyings[VARY_TANG]._VarName.." = tbn[0];\n"
		text = text.."    "..info.Varyings[VARY_BINORM]._VarName.." = tbn[1];\n"
		text = text.."    "..info.Varyings[VARY_NORM]._VarName.." = tbn[2];\n"
	else
		if info.Varyings[VARY_TANG] ~= nil then
			text = text.."    "..info.Varyings[VARY_TANG]._VarName.." = "..Tokens.Tangent.."; //asd\n"
		end
		if info.Varyings[VARY_BINORM] ~= nil then
			text = text.."    "..info.Varyings[VARY_BINORM]._VarName.." = "..Tokens.Binormal..";\n"			
		end
		if info.Varyings[VARY_NORM] ~= nil then
			text = text.."    "..info.Varyings[VARY_NORM]._VarName.." = "..Tokens.Normal..";\n"
		end
	end
	text = text.."\n"
	
	if info.TCoords.VertPreVars ~= nil then
		text = text..info.TCoords.VertPreVars
		text = text.."\n"
	end

	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			if info.TCoords[i].VertValueRefIdx == nil then
				if info.TCoords[i].VertPreVar ~= nil then
					text = text..info.TCoords[i].VertPreVar
				end
				text = text.."    VEC4 "..info.TCoords[i].VarName.." = "..info.TCoords[i].VertVarValue..";\n"
				if info.TCoords[i].VertPostVar ~= nil then
					text = text..info.TCoords[i].VertPostVar
				end
			end
		end
	end
	text = text.."\n"

	if info.TCoords.VertPostVars ~= nil then
		text = text..info.TCoords.VertPostVars
		text = text.."\n"
	end

	local ti = 0
	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			if info.TCoords[i].VertValueRefIdx == nil then
				if ti < 3 then
					text = text.."    "..info.Varyings[VARY_TC0 + ti]._VarName.." = VEC4("..info.TCoords[i].VarName..".xy, 0.0, 0.0);\n"
				else
					text = text.."    "..info.Varyings[VARY_TC0]._VarName..".w = tc3.x;\n"
					text = text.."    "..info.Varyings[VARY_TC1]._VarName..".w = tc3.y;\n"
				end
				ti = ti + 1
			end
		end
	end
	text = text.."\n"

	text = text..AppendVaryingsOut(info.Varyings)
	text = text.."\n"

	return text
end

function AppendPixelShaderBody(info, pass)
	local text = ""
	
	text = text..AppendVaryingsInVars(info.Varyings)
	text = text.."\n"

	if info.light or info.hasNMap then 
		if info.vnormal and info.perpixel then
			if info.light then
				text = text.."    VEC3 "..Tokens.Position.." = "..info.Varyings[VARY_POS]._VarName..".xyz;\n"
			end
			text = text.."    VEC3 eyeVec = normalize("..info.Varyings[VARY_EYE]._VarName..");\n"
			text = text.."\n"
		end
	end

	local ti = 0
	local paralax = nil
	local paralaxTCSrc = 0
	local paralaxTCDst = 0

	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			local idx = info.TCoords[i].LayerIdx
			if pass.Layers[idx]:getType() == vid.ETLT_NORMAL_MAP then
				local tchnl = info.TCoords[i].TChnl
				for j = 1, table.getn(info.TCoords) do
					if i ~= j and tchnl ~= -1 and tchnl == info.TCoords[j].TChnl then
						paralax = ""
						paralax = paralax.."    // texcoord offset (paralax effect)\n"
						paralax = paralax.."    FLOAT height = PARALAX_SCALE * TEX2D("..
							Uniforms.Tex[idx+1]..", "..info.TCoords[i].VarName..").a + PARALAX_BIAS;\n"
						paralax = paralax.."    "..info.TCoords[j].VarName.." += VEC2(eyeVec.x,-eyeVec.y) * height;\n"
						paralaxTCSrc = i
						paralaxTCDst = j
						break
					end
				end
			end
		end
		if paralax ~= nil then
			break
		end
	end

	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			text = text.."    VEC2 "..info.TCoords[i].VarName.." = "..info.TCoords[i].FragVarValue..";\n"
		end
	end
	text = text.."\n"

	if paralax ~= nil then
		text = text..paralax
		text = text.."\n"
	end

	local tnmapTC = 0

	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			local idx = info.TCoords[i].LayerIdx
			local tnmap = false
			if pass.Layers[idx]:getType() == vid.ETLT_NORMAL_MAP then
				if info.isDS or info.light then
					tnmap = true
				end
			else
				local tcol = string.format("tcol%d", idx)
				text = text.."    VEC4 "..tcol.." = TEX2D("..Uniforms.Tex[idx+1]..", "..info.TCoords[i].VarName..");\n"
			end
			if tnmap and tnmapTC == 0 then
				tnmapTC = i
			end
		end
	end
	text = text.."\n"

	if tnmapTC > 0 then
		local i = tnmapTC
		if paralaxTCSrc == i and paralaxTCDst > 0 then
			local j = paralaxTCDst
			if info.TCoords[i].FragVarValue == info.TCoords[j].VarName then
				text = text.."    "..info.TCoords[i].VarName.." = "..info.TCoords[i].FragVarValue..";\n"
				text = text.."\n"
			end
		end
		local idx = info.TCoords[i].LayerIdx
		text = text.."    VEC4 tnmap = TEX2D("..Uniforms.Tex[idx+1]..", "..info.TCoords[i].VarName..");\n"
		text = text.."\n"
	end

	local splatTbl = FetchTexColTbl(pass, vid.ETLT_SPLATTING_MAP)
	local diffTbl = FetchTexColTbl(pass, vid.ETLT_DIFFUSE_MAP)
	local lmapTbl = FetchTexColTbl(pass, vid.ETLT_LIGHT_MAP)
	local attenTbl = FetchTexColTbl(pass, vid.ETLT_ATTENUATION_MAP)

	if table.getn(diffTbl) > 0 or table.getn(lmapTbl) > 0 then
		text = text.."    VEC4 tcol = VEC4(1.0,1.0,1.0, 1.0);\n"
	end
		
	if table.getn(diffTbl) > 0 and table.getn(splatTbl) > 0 then
		text = text.."    tcol.a = "..splatTbl[1]..".a;\n"
		text = text.."    tcol.rgb = "
		for n = 1, table.getn(diffTbl) do
			text = text..diffTbl[n]..".rgb * "
			text = text..splatTbl[1]
			if n == 1 then
				text = text..".r"
			elseif n == 2 then
				text = text..".g"
			elseif n == 3 then
				text = text..".b"
			end
			if n < table.getn(diffTbl) then
				text = text.." + "
			else
				text = text..";\n"
			end
		end
		text = text.."\n"
	elseif table.getn(diffTbl) > 0 or table.getn(lmapTbl) > 0 then
		if table.getn(diffTbl) > 0 then
			text = text.."    tcol *= "
			for n = 1, table.getn(diffTbl) do
				text = text..diffTbl[n]
				if n < table.getn(diffTbl) then
					text = text.." * "
				else
					text = text..";\n"
				end
			end
		end
		text = text.."\n"
	end

	if table.getn(lmapTbl) > 0 then
		text = text.."    // lightmapping\n"
		text = text.."    tcol *= "
		for n = 1, table.getn(lmapTbl) do
			text = text..lmapTbl[n]
			if n < table.getn(lmapTbl) then
				text = text.." * "
			else
				text = text..";\n"
			end
		end
		text = text.."\n"
	end

	if table.getn(attenTbl) > 0 then
		text = text.."    VEC4 tatten = VEC4(1.0,1.0,1.0, 1.0);\n"
		text = text.."    tatten *= "
		for n = 1, table.getn(attenTbl) do
			text = text..attenTbl[n]
			if n < table.getn(attenTbl) then
				text = text.." * "
			else
				text = text..";\n"
			end
		end
		text = text.."\n"
	end

	if info.isDS then
		local N = Tokens.Normal
		local vN = info.Varyings[VARY_NORM]._VarName
		if info.hasNMap then
			local vT = info.Varyings[VARY_TANG]._VarName
			local vB = info.Varyings[VARY_BINORM]._VarName
			text = text.."    VEC3 "..N.." = (tnmap.xyz*2.0-1.0);\n"
			text = text.."    "..N.." = VEC3(dot("..N..","..vT.."), dot("..N..","..vB.."), dot("..N..","..vN.."));\n"
			text = text.."    "..N.." = normalize("..N..");\n"
			text = text.."    "..N.." = (N + 1.) / 2.;\n"
		else
			text = text.."    VEC3 "..N.." = normalize("..vN..");\n"
			text = text.."    "..N.." = (N + 1.) / 2.;\n"
		end
		text = text.."\n"
		text = text.."    VEC4 mDif = M_DIF("..Uniforms.MatColors..");\n"
		text = text.."    VEC4 mAmb = M_AMB("..Uniforms.MatColors..");\n"
		text = text.."    VEC4 color = mDif;\n"
		text = text.."\n"	
	elseif info.light and info.vnormal then
		if info.perpixel then
			if info.hasNMap then
				text = text.."    VEC3 "..Tokens.Normal.." = normalize((tnmap*2.0-1.0).xyz);\n"
			else
				text = text.."    VEC3 "..Tokens.Normal.." = normalize("..info.Varyings[VARY_NORM]._VarName..");\n"
			end
			text = text.."\n"
			text = text..AppendLighting(info, false, pass)
		else
			text = text.."    VEC3 specular = "..info.Varyings[VARY_SPEC]._VarName..";\n"
			text = text.."\n"
		end
	end

	if info.hasDiffMap or info.hasLightMap or info.hasAttenMap then
		if info.hasAttenMap then
			text = text.."    PS_OUT(FragData,0) = tatten;\n"
		end
		if info.hasDiffMap or info.hasLightMap then
			if info.hasAttenMap == false then
				text = text.."    PS_OUT(FragData,0) = tcol;\n"
			else
				text = text.."    PS_OUT(FragData,0) *= tcol;\n"
			end
		end
		if info.vcolor or info.mcolor or (info.light and info.vnormal and info.perpixel == false) then
			text = text.."    PS_OUT(FragData,0) *= "..info.Varyings[VARY_COL]._VarName..";\n"
		end
	elseif info.vcolor or info.mcolor then
		text = text.."    PS_OUT(FragData,0) = "..info.Varyings[VARY_COL]._VarName..";\n"
	else
  		text = text.."    PS_OUT(FragData,0) = VEC4(1.0,1.0,1.0,1.0);\n"
	end
	if info.light and info.vnormal and info.perpixel or info.isDS then
		text = text.."    PS_OUT(FragData,0) *= color;\n"
	end
	if info.light and info.vnormal then
		text = text.."    PS_OUT(FragData,0).rgb += specular;\n"
	end
	if info.fogging then
		text = text.."\n"
		text = text.."    // fog\n"
		text = text.."    FLOAT fdepth = "..info.Varyings[VARY_POS]._VarName..".z / "..info.Varyings[VARY_POS]._VarName..".w;\n"
		text = text.."    FLOAT fstart = FOG_START("..Uniforms.FogParams..");\n"
		text = text.."    FLOAT fend = FOG_END("..Uniforms.FogParams..");\n"
		text = text.."    FLOAT fog = clamp((fend - fdepth) / (fend - fstart), 0., 1.);\n"
		text = text.."    PS_OUT(FragData,0).rgb = MIX("..Uniforms.FogColor.."*PS_OUT(FragData,0).a, PS_OUT(FragData,0).rgb, fog);\n"
	end

	if info.isDS then
		if info.Varyings[VARY_NORM] ~= nil then
			text = text.."\n"
			text = text.."    PS_OUT(FragData,1) = VEC4("..Tokens.Normal..".rgb, 1.);\n"
		end
		if info.Varyings[VARY_POS] ~= nil then
			text = text.."\n"
			text = text.."    PS_OUT(FragData,2) = VEC4("..
				info.Varyings[VARY_POS]._VarName..".xyz,"..
				"1."..
				");\n"
		end
	end
	text = text.."\n"

	if info.isDS then
		if table.getn(lmapTbl) > 0 then
			text = text.."    PS_OUT(FragData,3).rgba = VEC4(0.,0.,0.,1.);\n"
		else
			text = text.."    PS_OUT(FragData,3).rgba = VEC4(1.,1.,1.,1.);\n"
		end
	end

	
	return text
end
