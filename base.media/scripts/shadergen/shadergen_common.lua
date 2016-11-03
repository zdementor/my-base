
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
	if bit.band(uniforms, vid.EUF_NORMAL_MATRIX) ~= 0 then
		text = text.."UNI MAT3 "..Uniforms.NormalMatrix..";\n"
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

function AppendVaryingVectorSpec(n)
	local ogl = MyDriver:getDriverFamily() == vid.EDF_OPENGL
	if ogl then
		return ""
	end
	return string.format(" : TEXCOORD%d", n);
end

function AppendVaryingColorSpec(n)
	local ogl = MyDriver:getDriverFamily() == vid.EDF_OPENGL
	if ogl then
		return ""
	end
	return string.format(" : COLOR%d", n);
end

function AppendVaryings(info)
	local text = ""
	for k, v in pairs(info.Varyings) do
		text = text..string.format("VARY "..v.Type.." "..v.VarName..v.Spec..";\n")
	end
	return text
end

function AppendVaryingsOutVars(varyings)
	local text = ""
	for k, v in pairs(varyings) do
		text = text..string.format("    "..v.Type.." "..v._VarName..";\n")
	end
	return text
end

function AppendVaryingsOut(varyings)
	local text = ""
	for k, v in pairs(varyings) do
		text = text..string.format("    VS_OUT("..v.VarName..") = "..v._VarName..";\n")
	end
	return text
end

function AppendVaryingsInVars(varyings)
	local text = ""
	for k, v in pairs(varyings) do
		text = text..string.format("    "..v.Type.." "..v._VarName.." = PS_IN("..v.VarName..");\n")
	end
	return text
end

function AppendMaterialDiffuse()
	local text = ""
	text = text.."    VEC4 mDif = M_DIF("..Uniforms.MatColors..");\n"
	return text
end

function AppendMaterialEmissive()
	local text = ""
	text = text.."    VEC4 mEmis = M_EMIS("..Uniforms.MatColors..");\n"
	return text
end

function AppendLighting(info, vsh, pass)
	local text = "\n"
	
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
				text = text.."        FLOAT lDist = length(lPos - position.xyz);\n"
			else
				text = text.."        VEC3 lVec = lPos - position.xyz;\n"
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
	text = text.."    VEC4 positionMVP = MUL(vertex,"..Uniforms.ModelViewProjMatrix..");\n"
	text = text.."\n"

	local normalDefined = false

	if IsNeedNormal(info.vtype, pass, info.lightcnt) then
		text = text.."    VEC4 position = MUL(vertex,"..Uniforms.ModelViewMatrix..");\n"
		text = text.."    VEC3 eyeVec = -position.xyz;\n"
		text = text.."    VEC3 "..Tokens.Normal.." = VS_IN("..Attribs.Normal..");\n"
		text = text.."    "..Tokens.Normal.." = MUL("..Tokens.Normal..","..Uniforms.NormalMatrix..");\n\n"
		if not info.perpixel then
			text = text.."    "..Tokens.Normal.." = normalize(N);\n"
			text = text.."    eyeVec = normalize(eyeVec);\n"
		end
		normalDefined = true
	elseif info.hasNMap then
		text = text.."    VEC4 position = MUL(vertex,"..Uniforms.ModelViewMatrix..");\n"
		text = text.."    VEC3 eyeVec = -position.xyz;\n"
	elseif info.fogging then
		text = text.."    VEC4 position = MUL(vertex,"..Uniforms.ModelViewMatrix..");\n"
	end

	if info.isDS and not normalDefined then
		if info.vnormal then
			text = text.."    VEC3 "..Tokens.Normal.." = VS_IN("..Attribs.Normal..");\n"
			text = text.."    "..Tokens.Normal.." = MUL("..Tokens.Normal..","..Uniforms.NormalMatrix..");\n\n"
		else
			text = text.."    VEC3 "..Tokens.Normal.." = VEC3(0., 1., 0.);\n"
		end
	end
	
	if info.light and info.vnormal and info.perpixel == false then
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
			text = text.."    "..Tokens.Tangent.."  = MUL("..Tokens.Tangent..","..Uniforms.NormalMatrix..");\n"
			text = text.."    "..Tokens.Binormal.." = MUL("..Tokens.Binormal..","..Uniforms.NormalMatrix..");\n"
			text = text.."    "..info.Varyings[VARY_EYE]._VarName..
				" = VEC3(dot(eyeVec, "..Tokens.Tangent.."), dot(eyeVec, "..Tokens.Binormal.."), dot(eyeVec, "..Tokens.Normal.."));\n"
			if pass:getLightingMode() ~= vid.ELM_NONE then
				text = text..string.format("    // transforming lights into TBN space\n")
				for i = 0, info.lightcnt - 1 do
					text = text..string.format("    { // Light %d\n", i)
					if info.lightcnt == 1 then
						text = text..string.format("        VEC3 lVec = L_POS("..Uniforms.Lighting..") - position.xyz;\n")
					else
						text = text..string.format("        VEC3 lVec = L_POS("..Uniforms.Lighting.."[%d]) - position.xyz;\n", i)
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
		if info.light then
			if info.hasNMap == false then
				text = text.."    "..info.Varyings[VARY_NORM]._VarName.." = "..Tokens.Normal..";\n"
				normalPassed = true
			end
			text = text.."    "..info.Varyings[VARY_POS]._VarName.." = position;\n"
		elseif info.fogging then
			text = text.."    "..info.Varyings[VARY_POS]._VarName.." = position;\n"
		end
	elseif info.fogging then
		text = text.."    "..info.Varyings[VARY_POS]._VarName.." = position;\n"
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

	if info.isDS then
		if info.Varyings[VARY_TANG] ~= nil then
			text = text.."    "..info.Varyings[VARY_TANG]._VarName.." = "..Tokens.Tangent..";\n"
		end
		if info.Varyings[VARY_BINORM] ~= nil then
			text = text.."    "..info.Varyings[VARY_BINORM]._VarName.." = "..Tokens.Binormal..";\n"
		end
		if not normalPassed then
			if info.Varyings[VARY_NORM] ~= nil then
				text = text.."    "..info.Varyings[VARY_NORM]._VarName.." = "..Tokens.Normal..";\n"
			end
		end
	end

	text = text.."\n"

	if info.TCoords.EnvGen ~= nil then
		text = text .. info.TCoords.EnvGen
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
				text = text.."    VEC3 position = "..info.Varyings[VARY_POS]._VarName..".xyz;\n"
			end
			text = text.."    VEC3 eyeVec = normalize("..info.Varyings[VARY_EYE]._VarName..");\n"
			text = text.."\n"
		end
	end

	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			text = text.."    VEC2 "..info.TCoords[i].VarName.." = "..info.TCoords[i].FragVarValue..";\n"
		end
	end
	text = text.."\n"
	
	if info.TCoords.Paralax ~= nil then
		text = text..info.TCoords.Paralax
		text = text.."\n"
	end

	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			local idx = info.TCoords[i].LayerIdx
			if info.light or pass.Layers[idx]:getType() ~= vid.ETLT_NORMAL_MAP then
				local tcol = ""
				if pass.Layers[idx]:getType() == vid.ETLT_NORMAL_MAP then
					tcol = "tnmap"
				else
					tcol = string.format("tcol%d", idx)
				end
				text = text.."    VEC4 "..tcol.." = TEX2D("..Uniforms.Tex[idx+1]..", "..info.TCoords[i].VarName..");\n"
			elseif info.isDS and info.hasNMap then
				local tcol = "tnmap"
				text = text.."    VEC4 "..tcol.." = TEX2D("..Uniforms.Tex[idx+1]..", "..info.TCoords[i].VarName..");\n"
			end
		end
	end
	text = text.."\n"

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
		text = text.."    //lightmapping\n"
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
			text = text.."    VEC3 "..N.." = normalize((tnmap*2.0-1.0).xyz);\n"
			text = text.."    "..vT.." = normalize("..vT..");\n"
			text = text.."    "..vB.." = normalize("..vB..");\n"
			text = text.."    "..vN.." = normalize("..vN..");\n"
			text = text.."    "..N.." = VEC3(dot("..N..","..vT.."), dot("..N..","..vB.."), dot("..N..","..vN.."));\n"
		text = text.."    "..N.." *= 0.001;\n"
		text = text.."    "..N.." += tnmap.xyz;\n"
		else
			text = text.."    VEC3 "..N.." = normalize("..vN..");\n"
		end
		--text = text.."    "..N.." = (N + 1.) / 2.;\n"
		text = text.."\n"	
	elseif info.light and info.vnormal then
		if info.perpixel then
			if info.hasNMap then
				text = text.."    VEC3 "..Tokens.Normal.." = normalize((tnmap*2.0-1.0).xyz);\n"
			else
				text = text.."    VEC3 "..Tokens.Normal.." = normalize("..info.Varyings[VARY_NORM]._VarName..");\n"
			end
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
	if info.light and info.vnormal and info.perpixel then
		text = text.."    PS_OUT(FragData,0) *= color;\n"
	end
	if info.light and info.vnormal then
		text = text.."    PS_OUT(FragData,0).rgb += specular;\n"
	end
	text = text.."\n"

	if info.fogging then
		text = text.."    // fog\n"
		text = text.."    FLOAT fdepth = "..info.Varyings[VARY_POS]._VarName..".z / "..info.Varyings[VARY_POS]._VarName..".w;\n"
		text = text.."    FLOAT fstart = FOG_START("..Uniforms.FogParams..");\n"
		text = text.."    FLOAT fend = FOG_END("..Uniforms.FogParams..");\n"
		text = text.."    FLOAT fog = clamp((fend - fdepth) / (fend - fstart), 0., 1.);\n"
		text = text.."\n"
		text = text.."    PS_OUT(FragData,0).rgb = MIX("..Uniforms.FogColor.."*PS_OUT(FragData,0).a, PS_OUT(FragData,0).rgb, fog);\n"
	end

	if info.isDS then
		if info.Varyings[VARY_NORM] ~= nil then
			text = text.."\n"
			text = text.."    PS_OUT(FragData,1).rgb *= 0.001;\n"
			text = text.."    PS_OUT(FragData,1).rgb += "..Tokens.Normal..".xyz;\n"
		end
	end
	
	return text
end
