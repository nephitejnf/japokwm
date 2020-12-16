require "defaultConfig"

sloppyFocus = true
borderPx = 2
rootColor = {0.3, 0.3, 0.3, 1.0}
borderColor = {0.3, 0.3, 0.3, 1.0}
focusColor = {1.0, 0.0, 0.0, 0.0}
overlayColor = {0.65, 0.65, 0.65, 0.9}
textColor = {0.003, 0.003, 0.003, 1.0}
selOverlayColor = {}
selTextColor = {}
outerGap = 10
innerGap = 20

tagNames = {"0:1", "1:2", "2:3", "3:4", "4:5", "5:6", "6:7", "7:8"}

-- where to put things
rules = {
    {"Gimp", "title", 1, true, 3}
}

layouts = {
    { "gf", function(n) loadLayout("tmp") end },
    {"[M]", function(n) monocle(n) end},
    {"[]=", function(n) tile(n) end},
    {"||", function(n) twoPane(n) end},
}

defaultLayout = layouts[1]

monrules = {
    -- name mfact nmaster scale layout transform
    { "rule", 0.55, 1, 1, layouts[1], NORMAL },
}

xkb_rules = {}
repeatRate = 25
repeatDelay = 600
termcmd = "/usr/bin/termite"

mod = mod1
-- maps (between 1 and 4)
keys = {
    {mod.."    "..shift.." Return",           function(n) action.spawn(termcmd) end},
    -- {mod.." period",      function(n) focusmon(1) end},
    -- {mod.." comma",       function(n) focusmon(-1) end},
    {mod.."     a",           function(n) action.setTabcount(action.getTabcount()+1) end},
    {mod.."    x",           function(n) action.setTabcount(action.getTabcount()-1) end},
    {mod.." k",           function(n) action.focusOnStack(-1) end},
    {mod.." j",           function(n) action.focusOnStack(1) end},
    {mod.." "..shift.." j",    function(n) action.focusOnHiddenStack(1) end},
    {mod.." "..shift.." k",    function(n) action.focusOnHiddenStack(-1) end},
    -- {mod.." d",           function(n) incnmaster(-1) end},
    {mod.." "..shift.." c",           function(n) action.kill() end},
    {mod.." "..shift.." q",           function(n) action.quit() end},
    {mod.." p",           function(n) splitThisContainer(1/2) end},
    {mod.." o",           function(n) vsplitThisContainer(1/2) end},
    {mod.." i",           function(n) mergeContainer(1, 1, 2) end},
    {mod.." space",       function(n) setLayout() end},
    {mod.." m",           function(n) setLayout(1) end},
    {mod.." "..shift.." t",           function(n) setLayout(2) end},
    {mod.." w",           function(n) setLayout(3) end},
    {mod.." b",           function(n) action.toggleConsiderLayerShell() end},
    {mod.." "..shift.." w",           function(n) setLayout(4) end},
    {mod.." "..shift.." l",           function(n) resizeThisAll(1/10, Direction.RIGHT) end},
    {mod.." "..shift.." h",           function(n) resizeThisAll(-(1/10), Direction.RIGHT) end},
    {mod.." "..shift.." s",    function(n) action.writeThisOverlay("tmp") end},
    -- {mod.." parenright",  function(n) tag(~0) end},
    -- {mod.." greater",     function(n) tagmon(1) end},
    -- {mod.." less",        function(n) tagmon(-1) end},
    {mod.." Return",      function(n) action.zoom() end},
    {mod.." s",           function(n) toggleOverlay() end},
    {mod.." 1",           function(n) action.view(1) end},
    {mod.." 2",           function(n) action.view(2) end},
    {mod.." 3",           function(n) action.view(4) end},
    {mod.." 4",           function(n) action.view(8) end},
    {mod.." 5",           function(n) action.view(16) end},
    {mod.." 6",           function(n) action.view(32) end},
    {mod.." 7",           function(n) action.view(64) end},
    {mod.." 8",           function(n) action.view(128) end},
    {mod.." 9",           function(n) action.view(256) end},
    {mod.." 0",           function(n) action.view(511) end},
    {mod.." "..shift.." 1",    function(n) action.toggleAddView(1) end},
    {mod.." "..shift.." 2",    function(n) action.toggleAddView(2) end},
    {mod.." "..shift.." 3",    function(n) action.toggleAddView(4) end},
    {mod.." "..shift.." 4",    function(n) action.toggleAddView(8) end},
    {mod.." "..shift.." 5",    function(n) action.toggleAddView(16) end},
    {mod.." "..shift.." 6",    function(n) action.toggleAddView(32) end},
    {mod.." "..shift.." 7",    function(n) action.toggleAddView(64) end},
    {mod.." "..shift.." 8",    function(n) action.toggleAddView(128) end},
    {mod.." "..shift.." 9",    function(n) action.toggleAddView(256) end},
    {mod.." "..shift.." r",    function(n) config.reload() end},
    {mod.." t",  function(n) action.setFloating(false)    end},
}

buttons = {
    {mod.." "..btnLeft,    function(n) action.moveResize(cursorMode.CurMove) end},
    {mod.." "..btnRight,   function(n) action.moveResize(cursorMode.CurResize) end},
}

print("init\n")
