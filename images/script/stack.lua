Stack = {}

function Stack:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    o.data = {}
    return o
end

function Stack:push(num)
    table.insert(self.data, num)
end

function Stack:pop()
    if #self.data ~= 0 then
        local ret = table.remove(self.data)
        return ret
    end
end

function Stack:top()
    local sz = #self.data
    if sz ~= 0 then
        return self.data[sz]
    end
end

function Stack:isEmpty()
    return (#self.data == 0)
end

-- 
StackQueue = Stack:new()

function StackQueue:insertBottom(num)
    table.insert(self.data, 1, num)
end