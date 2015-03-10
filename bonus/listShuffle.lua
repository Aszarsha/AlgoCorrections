#!/usr/bin/env lua

function initListAndMatrix (numElements)
	local l, pos = {}, {}
	for i = 1, numElements do
		l[i] = i
		pos[i] = {}
		for j = 1, numElements do pos[i][j] = 0 end
	end
	return l, pos
end

function printlist (l)
	for i = 1, #l do io.write(l[i] .. ' ') end
	io.write('\n')
end

function printmat (m)
	for i = 1, #m do printlist(m[i]) end
end

function listUpShuffle (l)
	local lsz = #l
	if lsz <= 1 then return l end

	local lsz2 = math.floor(lsz/2)
	local l1, l2 = {}, {}
	for k = 1, lsz2     do l1[#l1+1] = l[k] end
	for k = lsz2+1, lsz do l2[#l2+1] = l[k] end

	l1 = listUpShuffle(l1)
	l2 = listUpShuffle(l2)

	local res = {}
	local i, j = 1, 1
	while i <= #l1 or j <= #l2 do
		local rem1, rem2 = #l1-i+1, #l2-j+1
		if math.random() < rem1/(rem1+rem2) then
			res[#res+1] = l1[i]
			i = i+1
		else
			res[#res+1] = l2[j]
			j = j+1
		end
	end
	return res
end

function listDownShuffle (l)
	local lsz = #l
	if lsz <= 1 then return l end

	local lsz2 = math.floor(lsz/2)
	local l1, l2 = {}, {}
	for i = 1, lsz do
		local rem1, rem2 = lsz2-#l1, lsz-lsz2-#l2
		if math.random() < rem1/(rem1+rem2) then
			l1[#l1+1] = l[i]
		else
			l2[#l2+1] = l[i]
		end
	end

	l1 = listDownShuffle(l1)
	l2 = listDownShuffle(l2)

	local res = {}
	for i = 1, #l1 do res[#res+1] = l1[i] end
	for i = 1, #l2 do res[#res+1] = l2[i] end
	return res
end

math.randomseed(os.time())

local params = {...}
local numIterations = #params >= 1 and params[1] or 1
local numElements   = #params >= 2 and params[2] or 8

local l, pos = initListAndMatrix(numElements)
print("Up shuffle bias matrix:")
for i = 1, numIterations do
	local r = listUpShuffle(l)
	for j = 1, #r do
		pos[r[j]][j] = pos[r[j]][j]+1
	end
end
printmat(pos)

local l, pos = initListAndMatrix(numElements)
print("Down shuffle bias matrix:")
for i = 1, numIterations do
	local r = listDownShuffle(l)
	for j = 1, #r do
		pos[r[j]][j] = pos[r[j]][j]+1
	end
end
printmat(pos)
