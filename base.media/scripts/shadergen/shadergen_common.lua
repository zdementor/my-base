
Uniforms =
{
	GlobalAmbientColor	= vid.getUniformReadableName(vid.EUT_GLOBAL_AMBIENT_COLOR),
	MatColors			= vid.getUniformReadableName(vid.EUT_MATERIAL_COLORS),
	MatShininess		= vid.getUniformReadableName(vid.EUT_MATERIAL_SHININESS),
	Lighting			= vid.getUniformReadableName(vid.EUT_LIGHTING),
	ModelViewProjMatrix	= vid.getUniformReadableName(vid.EUT_MODEL_VIEW_PROJ_MATRIX),
	ModelViewMatrix		= vid.getUniformReadableName(vid.EUT_MODEL_VIEW_MATRIX),
	ModelMatrix			= vid.getUniformReadableName(vid.EUT_MODEL_MATRIX),
	NormalMatrix		= vid.getUniformReadableName(vid.EUT_NORMAL_MATRIX),
	Tex =
	{
		vid.getUniformReadableName(vid.EUT_TEXTURE0),
		vid.getUniformReadableName(vid.EUT_TEXTURE1),
		vid.getUniformReadableName(vid.EUT_TEXTURE2),
		vid.getUniformReadableName(vid.EUT_TEXTURE3),
	},
	TexMatrix =
	{
		vid.getUniformReadableName(vid.EUT_TEXTURE_MATRIX0),
		vid.getUniformReadableName(vid.EUT_TEXTURE_MATRIX1),
		vid.getUniformReadableName(vid.EUT_TEXTURE_MATRIX2),
		vid.getUniformReadableName(vid.EUT_TEXTURE_MATRIX3),
	},
	FogParams = vid.getUniformReadableName(vid.EUT_FOG_PARAMS),
	FogColor = vid.getUniformReadableName(vid.EUT_FOG_COLOR),
}

Attribs =
{
	Position  = vid.getAttribReadableName(vid.EAT_POSITION),
	Normal    = vid.getAttribReadableName(vid.EAT_NORMAL  ),
	Color     = vid.getAttribReadableName(vid.EAT_COLOR   ),
	TCoord0   = vid.getAttribReadableName(vid.EAT_TCOORD0 ),
	TCoord1   = vid.getAttribReadableName(vid.EAT_TCOORD1 ),
	TCoord2   = vid.getAttribReadableName(vid.EAT_TCOORD2 ),
	TCoord3   = vid.getAttribReadableName(vid.EAT_TCOORD3 ),
}

TexFlags =
{
	vid.EUF_TEXTURE0,
	vid.EUF_TEXTURE1,
	vid.EUF_TEXTURE2,
	vid.EUF_TEXTURE3,
}

TexMatrixFlags =
{
	vid.EUF_TEXTURE_MATRIX0,
	vid.EUF_TEXTURE_MATRIX1,
	vid.EUF_TEXTURE_MATRIX2,
	vid.EUF_TEXTURE_MATRIX3,
}

function HasNormal(vtype)
	local components = vid.getVertexComponents(vtype)
    if bit.band(components, vid.EVC_NRM) ~= 0 then
		return true
	end
	return false
end

function HasTBN(vtype)
	local components = vid.getVertexComponents(vtype)
    if bit.band(components, vid.EVC_TBN) ~= 0 then
		return true
	end
	return false
end

function HasColor(vtype)
	local components = vid.getVertexComponents(vtype)
    if bit.band(components, vid.EVC_COL) ~= 0 then
		return true
	end
	return false
end

function IsFogging(pass)
	if pass:getFlag(vid.EMF_FOG_ENABLE) then
		return true
	end
	return false
end

function IsLighting(pass, lightcnt)
	if lightcnt == nil then
		return false
	end
	if lightcnt < 1 then
		return false
	end
	if pass:isLighting() then
		return true
	end
	return false
end

function AppendDefines(vtype, pass, perpixel, lightcnt, uniforms)

	local text = ""
	local hasNMap = HasNMap(pass)
	local fogging = IsFogging(pass)

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
		text = text.."#define VS_OUT(vvv) vvv\n"
		text = text.."#define PS_IN(vvv) vvv\n"
		text = text.."#define PS_OUT(vvv,nnn) gl_##vvv[nnn]\n"
		text = text.."#define ATTR attribute\n"
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
		text = text.."#define VS_OUT(vvv) output.vvv\n"
		text = text.."#define PS_IN(vvv) input.vvv\n"
		text = text.."#define PS_OUT(vvv,nnn) output.##vvv##nnn\n"
		text = text.."#define ATTR\n"
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

	if hasNMap then
		text = text.."#define PARALAX_SCALE 0.03\n"
		text = text.."#define PARALAX_BIAS -PARALAX_SCALE/2.0\n"
		text = text.."\n"
	end

	if fogging then
		text = text.."#define FOG_START(fog)    fog.x\n"
		text = text.."#define FOG_END(fog)      fog.y\n"
		text = text.."#define FOG_DENSITY(fog)  fog.z\n"
		text = text.."\n"
	end

	return text
end

function AppendAttributes()
	local text=""
	local aParams = ShaderGenInfo.Attribs.Params
	local aMask = ShaderGenInfo.Attribs.Mask
	local aDecl = "ATTR "
	for atype = 0, vid.E_ATTRIB_TYPE_COUNT-1 do	
		if bit.band(aMask, vid.getAttribFlag(atype)) ~= 0 then
			text = text..aDecl..aParams[atype].Type.." "..aParams[atype].Name..aParams[atype].Spec..";\n"
		end
	end
	return text
end

function AppendUniforms(vtype, pass, perpixel, lightcnt, uniforms)

	local text = ""
	local ogl = MyDriver:getDriverFamily() == vid.EDF_OPENGL
	
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
		if lightcnt == 1 then
			text = text.."UNI MAT4 "..Uniforms.Lighting..";\n"
		else
			text = text.."UNI MAT4 "..Uniforms.Lighting.."["..lightcnt.."];\n"
		end
	end
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		if bit.band(uniforms, TexFlags[i+1]) ~=0 then
			if ogl then
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

function AppendVarying(vtype, pass, perpixel, lightcnt, uniforms)
	local text = ""
	local components = vid.getVertexComponents(vtype)
	local light = IsLighting(pass, lightcnt)
	local vnormal = HasNormal(vtype)
	local vcolor = HasColor(vtype)
	local mcolor = IsCustomVertexColor(vtype, pass)
	local hasNMap = HasNMap(pass)
	local fogging = IsFogging(pass)

	if vcolor or (light and vnormal and perpixel == false) or mcolor then
		text = text.."VARY VEC4 Color"..AppendVaryingColorSpec(0)..";\n"	
	end	
	if light and vnormal and perpixel == false then
		text = text.."VARY VEC3 Specular"..AppendVaryingColorSpec(1)..";\n"
	end

	local need_pos = false
	if fogging then
		need_pos = true
	end

	local tii = 0

	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		if ShaderGenInfo.TCoords[i].VarName then
			if ShaderGenInfo.TCoords[i].VertValueRefIdx == nil then
				text = text..string.format("VARY VEC4 TexCoord%d"..AppendVaryingVectorSpec(tii)..";\n", tii)
				tii = tii + 1
			end
		end
	end
	
	if light and vnormal and perpixel then
		if hasNMap == false then
			text = text..string.format("VARY VEC3 Normal"..AppendVaryingVectorSpec(tii)..";\n")
			tii = tii + 1
		end
		text = text..string.format("VARY VEC3 EyeVec"..AppendVaryingVectorSpec(tii)..";\n")
		tii = tii + 1
		need_pos = true
	elseif hasNMap and perpixel then
		text = text..string.format("VARY VEC3 EyeVec"..AppendVaryingVectorSpec(tii)..";\n")
		tii = tii + 1
	end
	if need_pos then
		text = text..string.format("VARY VEC4 Position"..AppendVaryingVectorSpec(tii)..";\n")
		tii = tii + 1
	end
	
	if light and hasNMap and HasTBN(vtype) and perpixel then
		if pass:getLightingMode() ~= vid.ELM_NONE then
			for i = 0, lightcnt - 1 do
				if i < 3 then
					text = text..string.format("VARY VEC4 LightVec%d"..AppendVaryingVectorSpec(tii)..";\n", i)
					tii = tii + 1
				end
			end
		end
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

function AppendLighting(pixel, vtype, pass, perpixel, lightcnt)

	local text = "\n"
	local hasAttenMap	= HasMap(pass, vid.ETLT_ATTENUATION_MAP)
	local hasNMap = HasNMap(pass)
	
	text = text..AppendMaterialDiffuse()
	text = text.."    VEC4 mAmbient = M_AMB("..Uniforms.MatColors..");\n"
	text = text.."    VEC4 mSpecular = M_SPEC("..Uniforms.MatColors..");\n"
	text = text..AppendMaterialEmissive()
	text = text.."\n"
	text = text.."    VEC3 specular = VEC3(0.0,0.0,0.0);\n"
	text = text.."    VEC4 color = VEC4(0.0, 0.0, 0.0, mDif.a);\n"
	text = text.."    color.rgb = "..Uniforms.GlobalAmbientColor..".rgb * mAmbient.rgb + mEmis.rgb;\n"
	if pass:getLightingMode() ~= vid.ELM_NONE then
		for i = 0, lightcnt - 1 do
			text = text..string.format("    // calculating lighting from Light %d\n", i)
			text = text.."    {\n"
			if lightcnt == 1 then
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
			if perpixel and HasTBN(vtype) and hasNMap then
				if i < 3 then
					text = text..string.format("        VEC3 lVec = normalize(PS_IN(LightVec%d).xyz);\n", i)
				else
					text = text..string.format("        VEC3 lVec = normalize(VEC3(PS_IN(LightVec0).w, PS_IN(LightVec1).w, PS_IN(LightVec2).w));\n", i)
				end
				text = text.."        FLOAT lDist = length(lPos - position.xyz);\n"
			else
				text = text.."        VEC3 lVec = lPos - position.xyz;\n"
				text = text.."        FLOAT lDist = length(lVec);\n"
				text = text..string.format("        lVec = normalize(lVec);\n")
			end
			text = text.."        FLOAT NdotL = max(dot(normal, lVec), 0.0);\n"
			text = text.."        FLOAT idx = min(lDist / lR, 1.0);\n"
			text = text.."        FLOAT atten = -idx * idx + 1.0;\n"
			text = text.."        color.rgb += atten * (\n"
			text = text.."            NdotL * mDif.rgb * lDif.rgb + // diffuse component\n"
			text = text.."            mAmbient.rgb * lAmb.rgb // ambient component\n"
			text = text.."            );\n"
			text = text.."        VEC3 reflVec = reflect(-lVec, normal);\n"
			text = text.."        FLOAT specPow = pow(max(dot(eyeVec, reflVec),0.0), "..Uniforms.MatShininess..");\n"
			text = text.."        // separate specular component\n"
			text = text.."        specular += atten * specPow * mSpecular.rgb * lSpec.rgb;\n"
			text = text.."    }\n"
		end
	end
	if pixel and hasAttenMap then
		text = text.."    color *= tatten;\n"
		text = text.."    specular *= tatten.rgb;\n"
	end
	text = text.."\n"
	return text
end

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

local function GetAttribParams(atype, specIdx)
	local adecl = {
		[vid.EAT_POSITION+1] = "VEC4",
		[vid.EAT_NORMAL+1]   = "VEC3",
		[vid.EAT_COLOR+1]    = "VEC4",
		[vid.EAT_TCOORD0+1]  = "VEC2",
		[vid.EAT_TCOORD1+1]  = "VEC2",
		[vid.EAT_TCOORD2+1]  = "VEC3",
		[vid.EAT_TCOORD3+1]  = "VEC3",

	}
	local aspecDX = {
		[vid.EAT_POSITION+1] = " : POSITION%s",
		[vid.EAT_NORMAL+1]   = " : NORMAL%s",
		[vid.EAT_COLOR+1]    = " : COLOR%s",
		[vid.EAT_TCOORD0+1]  = " : TEXCOORD%s",
		[vid.EAT_TCOORD1+1]  = " : TEXCOORD%s",
		[vid.EAT_TCOORD2+1]  = " : TEXCOORD%s",
		[vid.EAT_TCOORD3+1]  = " : TEXCOORD%s",

	}
	local idxStr = ""
	if specIdx ~= nil then
		idxStr = tostring(specIdx)
	end
	local aspec = ""
	if MyDriver:getDriverFamily() == vid.EDF_DIRECTX then
		aspec = string.format(aspecDX[atype+1], idxStr)
	end
	local aparams = {
		Mask = vid.getAttribFlag(atype),
		Type = adecl[atype+1],
		Name = vid.getAttribReadableName(atype),
		Spec = aspec,
	}
	return aparams
end

function GenInfo(vtype, pass, perpixel, lightcnt)
	local vsh	= ""
	local psh	= ""
	local light = IsLighting(pass, lightcnt)
	local vnormal = HasNormal(vtype)
	local mcolor = IsCustomVertexColor(vtype, pass)
	local hasNMap = HasNMap(pass)
	local fogging = IsFogging(pass)
	local vertUniforms = vid.EUF_NONE
	local fragUniforms = vid.EUF_NONE
	local attribs = 0
	local attribParams = {}
	local components = vid.getVertexComponents(vtype)

	ShaderGenInfo = {}
	
	-- attributes

	ShaderGenInfo.Attribs = {}

	local atype = vid.EAT_POSITION
	attribParams[atype] = GetAttribParams(atype)
	attribs = bit.bor(attribs, attribParams[atype].Mask)

	if bit.band(components, vid.EVC_NRM) ~= 0 then
		atype = vid.EAT_NORMAL
		attribParams[atype] = GetAttribParams(atype)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
	end
	if bit.band(components, vid.EVC_COL) ~= 0 then
		atype = vid.EAT_COLOR
		attribParams[atype] = GetAttribParams(atype, 0)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
	end
	local ti = 0
	if bit.band(components, vid.EVC_TC0) ~= 0 then
		atype = vid.EAT_TCOORD0
		attribParams[atype] = GetAttribParams(atype, ti)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
		ti = ti + 1
	end
	if bit.band(components, vid.EVC_TC1) ~= 0 then
		atype = vid.EAT_TCOORD1
		attribParams[atype] = GetAttribParams(atype, ti)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
		ti = ti + 1
	end
	if bit.band(components, vid.EVC_TBN) ~= 0 then
		atype = vid.EAT_TCOORD2
		attribParams[atype] = GetAttribParams(atype, ti)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
		ti = ti + 1
		atype = vid.EAT_TCOORD3
		attribParams[atype] = GetAttribParams(atype, ti)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
		ti = ti + 1
	end

	ShaderGenInfo.Attribs.Mask = attribs
	ShaderGenInfo.Attribs.Params = attribParams

	-- tcoords

	ShaderGenInfo.TCoords = {}

	-- Vertex
	local ti = 0
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		local texture = pass.Layers[i]:getTexture()
		if texture ~= nil and (pass.Layers[i]:getType() ~= vid.ETLT_NORMAL_MAP or perpixel) then
			local animated = false
			local tchnl = pass.Layers[i]:getTexCoordChannel()
			if (tchnl==1 and bit.band(ShaderGenInfo.Attribs.Mask, vid.EAF_TCOORD1)==0)
					or (tchnl==2 and bit.band(ShaderGenInfo.Attribs.Mask, vid.EAF_TCOORD2)==0)
					or (tchnl==3 and bit.band(ShaderGenInfo.Attribs.Mask, vid.EAF_TCOORD3)==0)
				 then
				tchnl=0
			end
			ShaderGenInfo.TCoords[ti + 1] = {}
			ShaderGenInfo.TCoords[ti + 1].VarName = string.format("tc%d", i)
			ShaderGenInfo.TCoords[ti + 1].LayerIdx = i
			ShaderGenInfo.TCoords[ti + 1].TChnl = -1
			if pass.Layers[i]:getTexCoordGen() == vid.ETCGT_ENVIRONMENT_MAPPING then
				if ShaderGenInfo.TCoords.TCEnv == nil then
					local text = ""
					text = text.."    // sphere map tcoords\n"
					text = text.."    VEC3 r = reflect(-eyeVec, normal);\n"
					text = text.."    FLOAT m = 2.0 * sqrt(r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0));\n"
					text = text..string.format("    VEC4 tcenv = VEC4(r.xy/m + 0.5, 1.0, 1.0);\n\n")
					ShaderGenInfo.TCoords.EnvGen = text
				end
				ShaderGenInfo.TCoords[ti + 1].VertPreVar = nil
				ShaderGenInfo.TCoords[ti + 1].VertVarValue = "tcenv"
			elseif pass.Layers[i]:getTexCoordGen() == vid.ETCGT_MANUAL_MAPPING then
				ShaderGenInfo.TCoords[ti + 1].VertPreVar = nil
				if tchnl==0 and bit.band(ShaderGenInfo.Attribs.Mask, vid.EAF_TCOORD0)==0 then
					ShaderGenInfo.TCoords[ti + 1].VertVarValue = "VEC4(0.0, 0.0, 1.0, 1.0)"
				else
					local tcoord = "TCoord"..tostring(tchnl)
					ShaderGenInfo.TCoords[ti + 1].VertVarValue = "VEC4(VS_IN("..Attribs[tcoord]..").xy, 1.0, 1.0)"
					ShaderGenInfo.TCoords[ti + 1].TChnl = tchnl
				end
			elseif pass.Layers[i]:getTexCoordGen() == vid.ETCGT_PROJECTED_MAPPING then
				local text = ""
				text = text.."    // calculating texture coords for projected mapping\n"
				if pass.Layers[i]:isTexCoordAnimated() then
					text = text.."    // and animating texture coords (translate/scale/rotate)\n"
					animated = true
				end
				ShaderGenInfo.TCoords[ti + 1].VertPreVar = text
				ShaderGenInfo.TCoords[ti + 1].VertVarValue = string.format("MUL(MUL(vertex,%s),%s)",
					Uniforms.ModelMatrix, Uniforms.TexMatrix[i+1])
			end
			if pass.Layers[i]:isTexCoordAnimated() and not animated then
				local text = ""
				text = text.."    // animating texture coords (translate/scale/rotate)\n"
				text = text..string.format("    tc%d = MUL(tc%d,%s);\n", i, i, Uniforms.TexMatrix[i+1])
				ShaderGenInfo.TCoords[ti + 1].VertPostVar = text
			end
			ti = ti + 1
		end
	end
	
	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		local tchnl = ShaderGenInfo.TCoords[i].TChnl
		local idx = ShaderGenInfo.TCoords[i].LayerIdx
		if pass.Layers[idx]:getType() == vid.ETLT_NORMAL_MAP then
			if not perpixel then
				ShaderGenInfo.TCoords[i].VarName = nil
			elseif not light then
				local used = 0
				for j = 1, table.getn(ShaderGenInfo.TCoords) do
					if i ~= j and tchnl ~= -1 and tchnl == ShaderGenInfo.TCoords[j].TChnl then
						used = used + 1
					end
				end
				if used == 0 then
					ShaderGenInfo.TCoords[i].VarName = nil
				end
			end
		end
	end
	
	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		if ShaderGenInfo.TCoords[i].VarName ~= nil then
			for j = i + 1, table.getn(ShaderGenInfo.TCoords) do
				if (ShaderGenInfo.TCoords[j].VertValueRefIdx == nil)
						and (ShaderGenInfo.TCoords[i].VertVarValue == ShaderGenInfo.TCoords[j].VertVarValue) then
					ShaderGenInfo.TCoords[j].VertValueRefIdx = i
				end
			end
		end
	end
	
	-- Pixel
	local ti = 0
	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		if ShaderGenInfo.TCoords[i].VarName ~= nil then
			local text = ""
			local idx = ShaderGenInfo.TCoords[i].LayerIdx
			if ShaderGenInfo.TCoords[i].VertValueRefIdx == nil then
				if ti < 3 then
					text = text..string.format("VEC2(PS_IN(TexCoord%d).xy)", ti)
				else
					text = text..string.format("VEC2(PS_IN(TexCoord0).w, PS_IN(TexCoord1).w)")
				end
				ti = ti + 1
			else
				text = text..ShaderGenInfo.TCoords[ShaderGenInfo.TCoords[i].VertValueRefIdx].VarName
			end
			ShaderGenInfo.TCoords[i].FragVarValue = text
			if pass.Layers[idx]:getType() == vid.ETLT_NORMAL_MAP and ShaderGenInfo.TCoords.Paralax == nil then
				local tchnl = ShaderGenInfo.TCoords[i].TChnl
				local text = ""
				text = text.."    // texcoord offset (paralax effect)\n"
				text = text.."    FLOAT height = PARALAX_SCALE * (TEX2D("..
					Uniforms.Tex[idx + 1]..", "..ShaderGenInfo.TCoords[i].VarName..").a) + PARALAX_BIAS;\n"
				for j = 1, table.getn(ShaderGenInfo.TCoords) do
					if i ~= j and tchnl ~= -1 and tchnl == ShaderGenInfo.TCoords[j].TChnl then
						text = text.."    "..ShaderGenInfo.TCoords[j].VarName.." += VEC2(eyeVec.x,-eyeVec.y) * height;\n"
					end
				end
				ShaderGenInfo.TCoords.Paralax = text
			end
		end
	end

	-- uniforms

	ShaderGenInfo.Uniforms = {}

	vertUniforms = bit.bor(vertUniforms,
		vid.EUF_MODEL_VIEW_PROJ_MATRIX)
		
	if IsNeedNormal(vtype, pass, lightcnt) then
		vertUniforms = bit.bor(vertUniforms,
			vid.EUF_MODEL_VIEW_MATRIX)
			
		vertUniforms = bit.bor(vertUniforms,
			vid.EUF_NORMAL_MATRIX)
		if light then
			if perpixel then
				fragUniforms = bit.bor(fragUniforms,
					vid.EUF_GLOBAL_AMBIENT_COLOR,
					vid.EUF_MATERIAL_COLORS, vid.EUF_MATERIAL_SHININESS,
					vid.EUF_LIGHTING)
				if HasTBN(vtype) then
					vertUniforms = bit.bor(vertUniforms,
						vid.EUF_LIGHTING)
				end
			else
				vertUniforms = bit.bor(vertUniforms,
					vid.EUF_GLOBAL_AMBIENT_COLOR,
					vid.EUF_MATERIAL_COLORS, vid.EUF_MATERIAL_SHININESS,
					vid.EUF_LIGHTING)
			end
		end
	elseif hasNMap then
		vertUniforms = bit.bor(vertUniforms,
			vid.EUF_MODEL_VIEW_MATRIX)
	elseif fogging then
		vertUniforms = bit.bor(vertUniforms,
			vid.EUF_MODEL_VIEW_MATRIX)
	end
	
	if mcolor then
		vertUniforms = bit.bor(vertUniforms,
			vid.EUF_MATERIAL_COLORS)
	end

	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		local idx = ShaderGenInfo.TCoords[i].LayerIdx
		if ShaderGenInfo.TCoords[i].VarName ~= nil then
			local texture = pass.Layers[idx]:getTexture()
			if texture ~= nil then
				if pass.Layers[idx]:getType() ~= vid.ETLT_NORMAL_MAP or perpixel then
					fragUniforms = bit.bor(fragUniforms, TexFlags[idx+1])
					if pass.Layers[idx]:isTexCoordAnimated() then
						vertUniforms = bit.bor(vertUniforms,
							TexMatrixFlags[idx+1])
					end
					if pass.Layers[idx]:getTexCoordGen() == vid.ETCGT_PROJECTED_MAPPING then
						vertUniforms = bit.bor(vertUniforms,
							TexMatrixFlags[idx+1], vid.EUF_MODEL_MATRIX)
					end
				end
			end
		end
	end

	if fogging then
		fragUniforms = bit.bor(fragUniforms,
			vid.EUF_FOG_PARAMS)
		fragUniforms = bit.bor(fragUniforms,
			vid.EUF_FOG_COLOR)
	end

	ShaderGenInfo.Uniforms.VertMask = vertUniforms
	ShaderGenInfo.Uniforms.FragMask = fragUniforms
	ShaderGenInfo.Uniforms.Mask = bit.bor(ShaderGenInfo.Uniforms.VertMask, ShaderGenInfo.Uniforms.FragMask)

end

function AppendVertShaderBody(vtype, pass, perpixel, lightcnt)

	local text = ""
	local light = IsLighting(pass, lightcnt)
	local vnormal = HasNormal(vtype)
	local vcolor = HasColor(vtype)
	local mcolor = IsCustomVertexColor(vtype, pass)
	local tcnum = GetTexCoordNum(vtype)
	local tnum = GetTexNum(pass)
	local hasNMap = HasNMap(pass)
	local fogging = IsFogging(pass)
	local hasSplatMap	= HasMap(pass, vid.ETLT_SPLATTING_MAP)

	text = text.."    VEC4 vertex = VS_IN("..Attribs.Position..");\n"
	text = text.."    VEC4 positionMVP = MUL(vertex,"..Uniforms.ModelViewProjMatrix..");\n"
	text = text.."\n"

	if IsNeedNormal(vtype, pass, lightcnt) then
		text = text.."    VEC4 position = MUL(vertex,"..Uniforms.ModelViewMatrix..");\n"
		text = text.."    VEC3 eyeVec = -position.xyz;\n"
		text = text.."    VEC3 normal = VS_IN("..Attribs.Normal..");\n"
		text = text.."    normal = MUL(normal,"..Uniforms.NormalMatrix..");\n\n"
		if not perpixel then
			text = text.."    normal = normalize(normal);\n"
			text = text.."    eyeVec = normalize(eyeVec);\n"
		end
	elseif hasNMap then
		text = text.."    VEC4 position = MUL(vertex,"..Uniforms.ModelViewMatrix..");\n"
		text = text.."    VEC3 eyeVec = -position.xyz;\n"
	elseif fogging then
		text = text.."    VEC4 position = MUL(vertex,"..Uniforms.ModelViewMatrix..");\n"
	end
	
	if light and vnormal and perpixel == false then
		text = text..AppendLighting(false, vtype, pass, perpixel, lightcnt)
		text = text.."    VS_OUT(Specular) = specular;\n"
	elseif mcolor then
		text = text..AppendMaterialDiffuse()
		text = text..AppendMaterialEmissive()
	end

	if vnormal and perpixel then
		if HasTBN(vtype) and hasNMap then
			text = text..string.format("    VEC3 tangent  = VS_IN("..Attribs.TCoord2..").xyz;\n")
			text = text..string.format("    VEC3 binormal = VS_IN("..Attribs.TCoord3..").xyz;\n")
			text = text..string.format("    tangent  = MUL(tangent,"..Uniforms.NormalMatrix..");\n")
			text = text..string.format("    binormal = MUL(binormal,"..Uniforms.NormalMatrix..");\n")
			text = text..string.format("    VS_OUT(EyeVec) = VEC3(\n", i)
			text = text.."        dot(eyeVec, tangent),\n"
			text = text.."        dot(eyeVec, binormal),\n"
			text = text.."        dot(eyeVec, normal));\n"
			if pass:getLightingMode() ~= vid.ELM_NONE then
				text = text..string.format("    // transforming lights into TBN space\n")
				for i = 0, lightcnt - 1 do
					text = text.."    {\n"
					text = text..string.format("    // Light %d\n", i)
					if lightcnt == 1 then
						text = text..string.format("        VEC3 lPos = L_POS("..Uniforms.Lighting..");\n")
					else
						text = text..string.format("        VEC3 lPos = L_POS("..Uniforms.Lighting.."[%d]);\n", i)
					end
					text = text.."        VEC3 lVec = lPos - position.xyz;\n"
					if i < 3 then
						text = text..string.format("        VS_OUT(LightVec%d) = VEC4(\n", i)
						text = text.."            dot(lVec, tangent),\n"
						text = text.."            dot(lVec, binormal),\n"
						text = text.."            dot(lVec, normal),\n"
						text = text.."            0.0);\n"
					else					
						text = text.."        VS_OUT(LightVec0).w = dot(lVec, tangent);\n"
						text = text.."        VS_OUT(LightVec1).w = dot(lVec, binormal);\n"
						text = text.."        VS_OUT(LightVec2).w = dot(lVec, normal);\n"
					end
					text = text.."    }\n"					
				end
			end
		elseif light or hasNMap then
			text = text.."    VS_OUT(EyeVec) = eyeVec;\n"
		end
		if light then
			if hasNMap == false then
				text = text.."    VS_OUT(Normal) = normal;\n"
			end
			text = text.."    VS_OUT(Position) = position;\n"
		elseif fogging then
			text = text.."    VS_OUT(Position) = position;\n"
		end
	elseif fogging then
		text = text.."    VS_OUT(Position) = position;\n"
	end
	if vcolor then
		if light and vnormal and perpixel == false then
			text = text.."    VS_OUT(Color) = VS_IN("..Attribs.Color..") * color;\n"
		else
			text = text.."    VS_OUT(Color) = VS_IN("..Attribs.Color..");\n"
		end
	elseif light and vnormal and perpixel == false then
		text = text.."    VS_OUT(Color) = color;\n"
	elseif mcolor then
		text = text.."    VS_OUT(Color) = VEC4(mEmis.rgb, mDif.a);\n"
	end
	text = text.."\n"

	if ShaderGenInfo.TCoords.EnvGen ~= nil then
		text = text .. ShaderGenInfo.TCoords.EnvGen
	end
	
	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		if ShaderGenInfo.TCoords[i].VarName ~= nil then
			if ShaderGenInfo.TCoords[i].VertValueRefIdx == nil then
				if ShaderGenInfo.TCoords[i].VertPreVar ~= nil then
					text = text..ShaderGenInfo.TCoords[i].VertPreVar
				end
				text = text.."    VEC4 "..ShaderGenInfo.TCoords[i].VarName.." = "..ShaderGenInfo.TCoords[i].VertVarValue..";\n"
				if ShaderGenInfo.TCoords[i].VertPostVar ~= nil then
					text = text..ShaderGenInfo.TCoords[i].VertPostVar
				end
			end
		end
	end
	text = text.."\n"

	local ti = 0
	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		if ShaderGenInfo.TCoords[i].VarName ~= nil then
			if ShaderGenInfo.TCoords[i].VertValueRefIdx == nil then
				if ti < 3 then
					text = text..string.format("    VS_OUT(TexCoord%d) = VEC4("..ShaderGenInfo.TCoords[i].VarName..".xy, 0.0, 0.0);\n", ti, i)
				else
					text = text..string.format("    VS_OUT(TexCoord0).w = tc3.x;\n")
					text = text..string.format("    VS_OUT(TexCoord1).w = tc3.y;\n")
				end
				ti = ti + 1
			end
		end
	end
	text = text.."\n"

	return text
end

function AppendPixelShaderBody(vtype, pass, perpixel, lightcnt)

	local text = ""
	local light = IsLighting(pass, lightcnt)
	local vnormal = HasNormal(vtype)
	local vcolor = HasColor(vtype)
	local mcolor = IsCustomVertexColor(vtype, pass)
	local tcnum = GetTexCoordNum(vtype)
	local tnum = GetTexNum(pass)
	local hasNMap		= HasMap(pass, vid.ETLT_NORMAL_MAP)
	local hasAttenMap	= HasMap(pass, vid.ETLT_ATTENUATION_MAP)
	local hasLightMap	= HasMap(pass, vid.ETLT_LIGHT_MAP)
	local hasDiffMap	= HasMap(pass, vid.ETLT_DIFFUSE_MAP)
	local hasSplatMap	= HasMap(pass, vid.ETLT_SPLATTING_MAP)
	local fogging = IsFogging(pass)
	
	if light or hasNMap then 
		if vnormal and perpixel then
			if light then
				text = text.."    VEC3 position = PS_IN(Position.xyz);\n"
			end
			text = text.."    VEC3 eyeVec = normalize(PS_IN(EyeVec));\n"
			text = text.."\n"
		end
	end

	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		if ShaderGenInfo.TCoords[i].VarName ~= nil then
			text = text.."    VEC2 "..ShaderGenInfo.TCoords[i].VarName.." = "..ShaderGenInfo.TCoords[i].FragVarValue..";\n"
		end
	end
	text = text.."\n"
	
	if ShaderGenInfo.TCoords.Paralax ~= nil then
		text = text..ShaderGenInfo.TCoords.Paralax
		text = text.."\n"
	end

	for i = 1, table.getn(ShaderGenInfo.TCoords) do
		if ShaderGenInfo.TCoords[i].VarName ~= nil then
			local idx = ShaderGenInfo.TCoords[i].LayerIdx
			if light or pass.Layers[idx]:getType() ~= vid.ETLT_NORMAL_MAP then
				local tcol = ""
				if pass.Layers[idx]:getType() == vid.ETLT_NORMAL_MAP then
					tcol = "tnmap"
				else
					tcol = string.format("tcol%d", idx)
				end
				text = text.."    VEC4 "..tcol.." = TEX2D("..Uniforms.Tex[idx+1]..", "..ShaderGenInfo.TCoords[i].VarName..");\n"
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
	
	if light and vnormal then
		if perpixel then
			if hasNMap then
				text = text.."    VEC3 normal = normalize((tnmap*2.0-1.0).xyz);\n"
			else
				text = text.."    VEC3 normal = normalize(PS_IN(Normal));\n"
			end		
			text = text..AppendLighting(true, vtype, pass, perpixel, lightcnt)
		else
			text = text.."    VEC3 specular = PS_IN(Specular);\n"
			text = text.."\n"
		end
	end

	if hasDiffMap or hasLightMap or hasAttenMap then
		if hasAttenMap then
			text = text.."    PS_OUT(FragData,0) = tatten;\n"
		end
		if hasDiffMap or hasLightMap then
			if hasAttenMap == false then
				text = text.."    PS_OUT(FragData,0) = tcol;\n"
			else
				text = text.."    PS_OUT(FragData,0) *= tcol;\n"
			end
		end
		if vcolor or mcolor or (light and vnormal and perpixel == false) then
			text = text.."    PS_OUT(FragData,0) *= PS_IN(Color);\n"
		end
	elseif vcolor or mcolor then
		text = text.."    PS_OUT(FragData,0) = PS_IN(Color);\n"
	else
  		text = text.."    PS_OUT(FragData,0) = VEC4(1.0,1.0,1.0,1.0);\n"
	end
	if light and vnormal and perpixel then
		text = text.."    PS_OUT(FragData,0) *= color;\n"
	end
	if light and vnormal then
		text = text.."    PS_OUT(FragData,0).rgb += specular;\n"
	end

	if fogging then
		text = text.."\n"
		text = text.."    // fogging\n"
		text = text.."    FLOAT fdepth = PS_IN(Position.z) / PS_IN(Position.w);\n"
		text = text.."    FLOAT fstart = FOG_START("..Uniforms.FogParams..");\n"
		text = text.."    FLOAT fend = FOG_END("..Uniforms.FogParams..");\n"
		text = text.."    FLOAT fog = clamp((fend - fdepth) / (fend - fstart), 0., 1.);\n"
		text = text.."    PS_OUT(FragData,0).rgb = MIX("..Uniforms.FogColor.."*PS_OUT(FragData,0).a, PS_OUT(FragData,0).rgb, fog);\n"
	end
	
	return text
end
		
function GetTexCoordNum(vertex_type)
    if vertex_type == vid.EVT_SIMPLE or
		vertex_type == vid.EVT_SIMPLE_COLOURED then
		return 0
	elseif vertex_type == vid.EVT_1TCOORDS or
		vertex_type == vid.EVT_1TCOORDS_COLOURED or
		vertex_type == vid.EVT_1TCOORDS_TBN or
		vertex_type == vid.EVT_1TCOORDS_TBN_COLOURED or
		vertex_type == vid.EVT_1TCOORDS_RHW_COLOURED then
		return 1
	elseif vertex_type == vid.EVT_2TCOORDS or
		vertex_type == vid.EVT_2TCOORDS_COLOURED or
		vertex_type == vid.EVT_2TCOORDS_TBN or
		vertex_type == vid.EVT_2TCOORDS_TBN_COLOURED then
		return 2
	end
	return 0
end

function  HasMap(pass, textype)
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		local texture = pass.Layers[i]:getTexture()
		if texture ~= nil and pass.Layers[i]:getType() == textype then
			return true
		end
	end
	return false
end

function HasNMap(pass)
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		local texture = pass.Layers[i]:getTexture()
		if texture ~= nil and pass.Layers[i]:getType() == vid.ETLT_NORMAL_MAP then
			return true
		end
	end
	return false
end

function IsCustomVertexColor(vtype, pass)
	local vcolor = HasColor(vtype)
	if vcolor then
		return false
	end
	if pass:getColorGen() ~= vid.ECGT_VERTEX or pass:getAlphaGen() ~= vid.EAGT_VERTEX then
		return true
	end
	return false
end

function IsNeedNormal(vtype, pass, lightcnt)
	local light = IsLighting(pass, lightcnt)
	local vnormal = HasNormal(vtype)
	if not vnormal then
		return false
	end
	if light then
		return true
	end
	if HasTBN(vtype) and HasNMap(pass) then
		return true
	end
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		local texture = pass.Layers[i]:getTexture()
		if texture ~= nil and pass.Layers[i]:getTexCoordGen() ==
			vid.ETCGT_ENVIRONMENT_MAPPING then
			return true
		end
	end	
	return false
end

function GetTexNum(pass)
	local texnum = 0
    for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		local texture = pass.Layers[i]:getTexture()
		if texture ~= nil  then
			texnum = texnum + 1
		end
	end	
	return texnum
end
