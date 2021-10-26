-- some options you might want to change
opt.automatic_workspace_naming = true;
opt.sloppy_focus = true
opt.workspaces = {"0:1", "1:2", "2:3", "3:4", "4:5", "5:6", "6:7", "7:8"}
opt.inner_gaps = 15
opt.border_color = Color.new(0.0, 0.0, 1.0, 1.0)
opt.focus_color = Color.new(1.0, 0.0, 0.0, 1.0)

local function on_start()
    -- execute programs or do what ever you want e.g.:
    -- action.exec("dbus-daemon --session --address=unix:path=$XDG_RUNTIME_DIR/bus")
    -- action.exec("xsetroot -cursor_name left_ptr")
end
-- executes function on_start when the 
event:add_listener("on_start", on_start)

local layouts = {"two_pane", "tile"}

opt.create_layout_set("default", layouts)

layout.default_layout = "two_pane"

-- set it to 4 to use super instead
opt.mod = 1

local function exec_keycombo(i)
    if (info.is_keycombo("combo")) then
        Workspace.get_focused().tags:_xor(1 << (i-1))
    else
        action.view(Workspace.get(i))
    end
    action.start_keycombo("combo")
end

local function toggle_all_bars()
    for i = 1,info.get_workspace_count() do
        local ws_id = i-1
        local ws = server:get_workspace(ws_id)
        action.toggle_bars(ws, Direction.all)
    end
end

opt:bind_key("mod-S-p",       function() container.set_sticky(info.this_container_position(), 255) end)
opt:bind_key("mod-e",         function() action.view(Workspace.get_focused():get_next_empty(Direction.left):get_id()) end)
opt:bind_key("mod-period",    function() action.toggle_workspace() end)
opt:bind_key("mod-S-period",  function() action.toggle_tags() end)
opt:bind_key("mod-k",         function() action.focus_on_stack(-1) end)
opt:bind_key("mod-j",         function() action.focus_on_stack(1) end)
opt:bind_key("mod-S-j",       function() action.focus_on_hidden_stack(0) end)
opt:bind_key("mod-S-k",       function() action.focus_on_hidden_stack(-1) end)
opt:bind_key("mod-tab",       function() action.swap_on_hidden_stack(0) end)
opt:bind_key("mod-S-tab",     function() action.swap_on_hidden_stack(-1) end)
opt:bind_key("mod-space",     function() Layout.load_next_in_set("default") end)
opt:bind_key("mod-minus",     function() action.move_to_scratchpad(info.this_container_position()) end)
opt:bind_key("mod-S-minus",   function() action.show_scratchpad() end)
opt:bind_key("mod-S-space",   function() action.load_prev_layout_in_set("default") end)
opt:bind_key("mod-m",         function() action.focus_container(info.stack_position_to_position(0)) end)
opt:bind_key("mod-S-t",       function() action.load_layout_in_set("default", 2) end)
opt:bind_key("mod-w",         function() action.load_layout_in_set("default", 3) end)
opt:bind_key("mod-S-w",       function() action.load_layout_in_set("default", 4) end)
opt:bind_key("mod-b",         function() toggle_all_bars() end)
opt:bind_key("mod-S-h",       function() action.resize_main(-1/10) end)
opt:bind_key("mod-S-l",       function() action.resize_main(1/10) end)
opt:bind_key("mod-Return",    function() action.zoom() end)
opt:bind_key("mod-1",         function() exec_keycombo(1) end)
opt:bind_key("mod-2",         function() exec_keycombo(2) end)
opt:bind_key("mod-3",         function() exec_keycombo(3) end)
opt:bind_key("mod-4",         function() exec_keycombo(4) end)
opt:bind_key("mod-5",         function() exec_keycombo(5) end)
opt:bind_key("mod-6",         function() exec_keycombo(6) end)
opt:bind_key("mod-7",         function() exec_keycombo(7) end)
opt:bind_key("mod-8",         function() exec_keycombo(8) end)
opt:bind_key("mod-9",         function() exec_keycombo(9) end)
opt:bind_key("mod-0",         function() Workspace.get_focused().tags = 1 << Workspace.get_focused():get_id() end)
opt:bind_key("mod-S-0",       function()
    local con = Container.get_focused()
    if con then
        Workspace.get_focused().tags = 1 << con.workspace:get_id()
    end
end)
opt:bind_key("mod-S-9",       function()
    if Container.get_focused() then
        Container.get_focused().workspace = Workspace.get(9)
    end
end)
opt:bind_key("mod-C-S-0",     function() Container.get_focused():set_sticky_restricted(0) end)
opt:bind_key("mod-C-S-9",     function() Container.get_focused():set_sticky_restricted(255) end)
