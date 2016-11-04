
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

VARY_POS   = 1
VARY_COL   = 2
VARY_SPEC  = 3
VARY_TC0   = 4
VARY_TC1   = 5
VARY_TC2   = 6
VARY_TANG  = 7
VARY_BINORM= 8
VARY_NORM  = 9
VARY_EYE   = 10
VARY_LVEC0 = 11
VARY_LVEC1 = 12
VARY_LVEC2 = 13
VARY_COUNT = 14

Varyings =
{
	[VARY_POS   ] = "vPos",
	[VARY_COL   ] = "vCol",
	[VARY_SPEC  ] = "vSpec",
	[VARY_TC0   ] = "vTC0",
	[VARY_TC1   ] = "vTC1",
	[VARY_TC2   ] = "vTC2",
	[VARY_TANG  ] = "vT",
	[VARY_BINORM] = "vB",
	[VARY_NORM  ] = "vN",
	[VARY_EYE   ] = "vEye",
	[VARY_LVEC0 ] = "vLVec0",
	[VARY_LVEC1 ] = "vLVec1",
	[VARY_LVEC2 ] = "vLVec2",
}

Attribs =
{
	Position  = vid.getAttribReadableName(vid.EAT_POSITION),
	Normal    = vid.getAttribReadableName(vid.EAT_NORMAL  ),
	Color     = vid.getAttribReadableName(vid.EAT_COLOR   ),
	TCoord0   = vid.getAttribReadableName(vid.EAT_TCOORD0 ),
	TCoord1   = vid.getAttribReadableName(vid.EAT_TCOORD1 ),
	Tangent   = vid.getAttribReadableName(vid.EAT_TANGENT ),
	Binormal  = vid.getAttribReadableName(vid.EAT_BINORMAL),
}

Tokens =
{
	Tangent = "T",
	Binormal= "B",
	Normal  = "N",
	Position = "pos",
	PositionMVP = "posMVP",
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

local function GetAttribParams(atype, specIdx)
	local adecl = {
		[vid.EAT_POSITION+1] = "VEC4",
		[vid.EAT_NORMAL+1]   = "VEC3",
		[vid.EAT_COLOR+1]    = "VEC4",
		[vid.EAT_TCOORD0+1]  = "VEC2",
		[vid.EAT_TCOORD1+1]  = "VEC2",
		[vid.EAT_TANGENT+1]  = "VEC3",
		[vid.EAT_BINORMAL+1] = "VEC3",
	}
	local aspecDX = {
		[vid.EAT_POSITION+1] = " : POSITION%s",
		[vid.EAT_NORMAL+1]   = " : NORMAL%s",
		[vid.EAT_COLOR+1]    = " : COLOR%s",
		[vid.EAT_TCOORD0+1]  = " : TEXCOORD%s",
		[vid.EAT_TCOORD1+1]  = " : TEXCOORD%s",
		[vid.EAT_TANGENT+1]  = " : TANGENT%s",
		[vid.EAT_BINORMAL+1] = " : BINORMAL%s",
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

function ShaderGenInfo(vtype, pass, perpixel, lightcnt)

	local vsh	= ""
	local psh	= ""
	local light = IsLighting(pass, lightcnt)
	local vnormal = HasNormal(vtype)
	local vcolor = HasColor(vtype)
	local mcolor = IsCustomVertexColor(vtype, pass)
	local hasNMap = HasNMap(pass)
	local hasTBN = HasTBN(vtype)
	local fogging = IsFogging(pass)
	local components = vid.getVertexComponents(vtype)
	local isDS = MyDriver:getRenderPath() == vid.ERP_DEFERRED_SHADING

	local vertUniforms = vid.EUF_NONE
	local fragUniforms = vid.EUF_NONE
	local attribs = 0
	local attribParams = {}

	local info = {}
	info.vtype = vtype
	info.perpixel = perpixel
	info.light = light
	info.lightcnt = lightcnt
	info.vnormal = vnormal
	info.vcolor = vcolor
	info.mcolor = mcolor
	info.hasNMap = hasNMap
	info.hasTBN = hasTBN
	info.hasAttenMap = HasMap(pass, vid.ETLT_ATTENUATION_MAP)
	info.hasLightMap = HasMap(pass, vid.ETLT_LIGHT_MAP)
	info.hasDiffMap  = HasMap(pass, vid.ETLT_DIFFUSE_MAP)
	info.hasSplatMap = HasMap(pass, vid.ETLT_SPLATTING_MAP)
	info.fogging = fogging
	info.components = components
	info.ogl = MyDriver:getDriverFamily() == vid.EDF_OPENGL
	info.tcnum = GetTexCoordNum(vtype)
	info.tnum = GetTexNum(pass)
	info.isDS = isDS

	-- attributes

	info.Attribs = {}

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
		atype = vid.EAT_TANGENT
		attribParams[atype] = GetAttribParams(atype)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
		atype = vid.EAT_BINORMAL
		attribParams[atype] = GetAttribParams(atype)
		attribs = bit.bor(attribs, attribParams[atype].Mask)
	end

	info.Attribs.Mask = attribs
	info.Attribs.Params = attribParams

	-- tcoords

	info.TCoords = {}

	-- Vertex
	local envGenText = nil
	local ti = 0
	for i = 0, vid.MY_MATERIAL_MAX_LAYERS-1 do
		local texture = pass.Layers[i]:getTexture()
		if texture ~= nil and (pass.Layers[i]:getType() ~= vid.ETLT_NORMAL_MAP or perpixel) then
			local animated = false
			local tchnl = pass.Layers[i]:getTexCoordChannel()
			if (tchnl==1 and bit.band(info.Attribs.Mask, vid.EAF_TCOORD1)==0)
				 then
				tchnl=0
			end
			info.TCoords[ti + 1] = {}
			info.TCoords[ti + 1].VarName = string.format("tc%d", i)
			info.TCoords[ti + 1].LayerIdx = i
			info.TCoords[ti + 1].TChnl = -1
			if pass.Layers[i]:getTexCoordGen() == vid.ETCGT_ENVIRONMENT_MAPPING then
				local text = ""
				if envGenText == nil then
					envGenText = ""
					envGenText = envGenText.."    // sphere maping\n"
					envGenText = envGenText.."    VEC3 r = reflect(-eyeVec, "..Tokens.Normal..");\n"
					envGenText = envGenText.."    FLOAT m = 2.0 * sqrt(r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0));\n"
					envGenText = envGenText.."    VEC4 tcenv = VEC4(r.xy/m + 0.5, 1.0, 1.0);\n"
				end
				info.TCoords[ti + 1].VertPreVar = text
				info.TCoords[ti + 1].VertVarValue = "tcenv"
			elseif pass.Layers[i]:getTexCoordGen() == vid.ETCGT_MANUAL_MAPPING then
				info.TCoords[ti + 1].VertPreVar = nil
				if tchnl==0 and bit.band(info.Attribs.Mask, vid.EAF_TCOORD0)==0 then
					info.TCoords[ti + 1].VertVarValue = "VEC4(0.0, 0.0, 1.0, 1.0)"
				else
					local tcoord = "TCoord"..tostring(tchnl)
					info.TCoords[ti + 1].VertVarValue = "VEC4(VS_IN("..Attribs[tcoord]..").xy, 1.0, 1.0)"
					info.TCoords[ti + 1].TChnl = tchnl
				end
			elseif pass.Layers[i]:getTexCoordGen() == vid.ETCGT_PROJECTED_MAPPING then
				local text = ""
				text = text.."    // projected mapping\n"
				if pass.Layers[i]:isTexCoordAnimated() then
					text = text.."    // animating tex coords (translate/scale/rotate)\n"
					animated = true
				end
				info.TCoords[ti + 1].VertPreVar = text
				info.TCoords[ti + 1].VertVarValue = string.format("MUL(MUL(vertex,%s),%s)",
					Uniforms.ModelMatrix, Uniforms.TexMatrix[i+1])
			end
			if pass.Layers[i]:isTexCoordAnimated() and not animated then
				local text = ""
				text = text.."    // animating tex coords (translate/scale/rotate)\n"
				text = text..string.format("    tc%d = MUL(tc%d,%s);\n", i, i, Uniforms.TexMatrix[i+1])
				info.TCoords[ti + 1].VertPostVar = text
			end
			ti = ti + 1
		end
	end
	if envGenText then
		info.TCoords.VertPreVars = envGenText
	end
	info.TCoords.VertPostVars = nil
	
	for i = 1, table.getn(info.TCoords) do
		local tchnl = info.TCoords[i].TChnl
		local idx = info.TCoords[i].LayerIdx
		if pass.Layers[idx]:getType() == vid.ETLT_NORMAL_MAP then
			if not perpixel then
				info.TCoords[i].VarName = nil
			elseif not light then
				local used = 0
				for j = 1, table.getn(info.TCoords) do
					if i ~= j and tchnl ~= -1 and tchnl == info.TCoords[j].TChnl then
						used = used + 1
					end
				end
				if used == 0 then
					info.TCoords[i].VarName = nil
				end
			end
		end
	end
	
	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			for j = i + 1, table.getn(info.TCoords) do
				if (info.TCoords[j].VertValueRefIdx == nil)
						and (info.TCoords[i].VertVarValue == info.TCoords[j].VertVarValue) then
					info.TCoords[j].VertValueRefIdx = i
				end
			end
		end
	end

	-- uniforms

	info.Uniforms = {}

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

	if isDS and vnormal then
		vertUniforms = bit.bor(vertUniforms,
			vid.EUF_NORMAL_MATRIX)
	end
	
	if mcolor then
		vertUniforms = bit.bor(vertUniforms,
			vid.EUF_MATERIAL_COLORS)
	end

	for i = 1, table.getn(info.TCoords) do
		local idx = info.TCoords[i].LayerIdx
		if info.TCoords[i].VarName ~= nil then
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

	info.Uniforms.VertMask = vertUniforms
	info.Uniforms.FragMask = fragUniforms
	info.Uniforms.Mask = bit.bor(info.Uniforms.VertMask, info.Uniforms.FragMask)

	-- varyings

	local varyings = {}

	if vcolor or (light and vnormal and perpixel == false) or mcolor then
		local v = {}
		v.Type = "VEC4"	
		v.VarName = Varyings[VARY_COL]
		varyings[VARY_COL] = v
	end	
	if light and vnormal and perpixel == false then
		local v = {}
		v.Type = "VEC3"	
		v.VarName = Varyings[VARY_SPEC]
		varyings[VARY_SPEC] = v
	end	

	local need_pos = false
	if fogging then
		need_pos = true
	end

	local tii = 0

	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName then
			if info.TCoords[i].VertValueRefIdx == nil then
				local v = {}
				v.Type = "VEC4"	
				v.VarName = Varyings[VARY_TC0 + tii]
				varyings[VARY_TC0 + tii] = v
				tii = tii + 1
			end
		end
	end

	if light and vnormal and perpixel then
		if hasNMap == false then
			local v = {}
			v.Type = "VEC3"	
			v.VarName = Varyings[VARY_NORM]
			varyings[VARY_NORM] = v
		end
		local v = {}
		v.Type = "VEC3"	
		v.VarName = Varyings[VARY_EYE]
		varyings[VARY_EYE] = v
		need_pos = true
	elseif hasNMap and perpixel then
		local v = {}
		v.Type = "VEC3"	
		v.VarName = Varyings[VARY_EYE]
		varyings[VARY_EYE] = v
	end

	if need_pos then
		local v = {}
		v.Type = "VEC4"	
		v.VarName = Varyings[VARY_POS]
		varyings[VARY_POS] = v
	end

	if isDS then
		if hasNMap and hasTBN then
			local v = {}
			v.Type = "VEC3"	
			v.VarName = Varyings[VARY_TANG]
			varyings[VARY_TANG] = v
			v = {}
			v.Type = "VEC3"	
			v.VarName = Varyings[VARY_BINORM]
			varyings[VARY_BINORM] = v
		end
		if varyings[VARY_NORM] == nil then
			local v = {}
			v.Type = "VEC3"	
			v.VarName = Varyings[VARY_NORM]
			varyings[VARY_NORM] = v
		end
	end

	if light and hasNMap and HasTBN(vtype) and perpixel then
		if pass:getLightingMode() ~= vid.ELM_NONE then
			for i = 0, lightcnt - 1 do
				if i < 3 then
					local v = {}
					v.Type = "VEC4"	
					v.VarName = Varyings[VARY_LVEC0 + i]
					varyings[VARY_LVEC0 + i] = v
				end
			end
		end
	end

	for k, v in pairs(varyings) do
		v._VarName = "_"..v.VarName
	end

	info.Varyings = varyings

	-- Pixel TCoords
	local ti = 0
	for i = 1, table.getn(info.TCoords) do
		if info.TCoords[i].VarName ~= nil then
			local text = ""
			local idx = info.TCoords[i].LayerIdx
			if info.TCoords[i].VertValueRefIdx == nil then
				if ti < 3 then
					text = text.."VEC2("..varyings[VARY_TC0 + ti]._VarName..".xy)"
				else
					text = text.."VEC2("..
						varyings[VARY_TC0]._VarName..".w, "..
						varyings[VARY_TC1]._VarName..".w)"
				end
				ti = ti + 1
			else
				text = text..info.TCoords[info.TCoords[i].VertValueRefIdx].VarName
			end
			info.TCoords[i].FragVarValue = text
		end
	end

	return info

end
