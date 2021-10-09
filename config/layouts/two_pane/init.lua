local layout_data = {
    {
        {0, 0, 1, 1},
    },
    {
        {0.0, 0.0, 0.5, 1.0},
        {0.5, 0.0, 0.5, 1.0},
    },
}

local function update(n)
    if n <= 1 then
        l.config.set_tile_borderpx(0)
    else
        l.config.set_tile_borderpx(2)
    end
end

layout.set(layout_data)
-- l.event.add_listener("on_update", update)
opt.set_master_layout_data(
{{{0, 0, 1, 1}}, {{0, 0, 0.5, 1}, {0.5, 0, 0.5, 1}}}
)
opt.hidden_edges = info.direction.all
opt.inner_gaps = 0
opt.outer_gaps = 0
opt.resize_direction = info.direction.right
opt:set_layout_constraints({min_width = 0.1, max_width = 1, min_height = 0.1, max_height = 1})
opt:set_master_constraints({min_width = 0.2, max_width = 1, min_height = 0.2, max_height = 1})
