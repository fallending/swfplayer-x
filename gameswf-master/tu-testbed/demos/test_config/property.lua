-- property.lua

-- Code for Lua property sets.


function set_property(propertyset, k, v)
	local property = rawget(propertyset, k);
	if type(property) == "table" then
		property_set_value(property, v)
	else
		rawset(propertyset, k, v)
	end
end


function get_property(t, k)
	local property = rawget(t, k);
	if type(property) == "table" then
		return rawget(property, "value")
	elseif property == nil and rawget(t, "defaults") ~= nil then
		-- chain back to a backup table, if any.
		return get_property(rawget(t, "defaults"), k)
	else
		return property
	end
end


function property_set_value(p, v)
-- Called to set the value of a property.  Check min/max or options to
-- validate the new data.

	if p.min ~= nil or p.max ~= nil then
		-- numerical property
		v = tonumber(v)
		if p.min ~= nil and v < p.min then
			v = p.min
		end
		if p.max ~= nil and v > p.max then
			v = p.max
		end
		p.value = v

	elseif type(p.options) == 'table' then
		-- enumerated property
		foreach(p.options, function (_, val)
			if val == %v then
				%p.value = val
				return 1	-- break out of iteration
			end
		end)
--		if p.value ~= val then warning(....) end		
	else
		-- unspecified property
		-- warn???
		p.value = v;
	end
end


function property_set(t)
-- constructor for a set of properties
	if property_set_tag == nil then
		property_set_tag = newtag()
		settagmethod(property_set_tag, "settable", set_property)
		settagmethod(property_set_tag, "gettable", get_property)
	end
	settag(t, property_set_tag)
	return t;
end
