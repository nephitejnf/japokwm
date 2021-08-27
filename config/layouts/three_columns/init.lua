local layout_data = {
    {
        {0.0, 0.0, 1.0, 1},
    },
    {
        {0.0, 0.0, 0.5, 1},
        {0.5, 0.0, 0.5, 1},
    },
    {
        {0.0, 0.0, 0.5, 1},
        {0.5, 0.0, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
    },
    {
        {0.0, 0.0, 0.5, 1},
        {0.5, 0.0, 0.5, 0.333},
        {0.5, 0.333, 0.5, 0.333},
        {0.5, 0.666, 0.5, 0.333},
    },
    {
        {0.0, 0.0, 0.55, 1},
        {0.55, 0.0, 0.225, 0.333},
        {0.55, 0.333, 0.225, 0.333},
        {0.55, 0.666, 0.225, 0.333},
        {0.775, 0.0, 0.225, 1},
    },
    {
        {0.0, 0.0, 0.55, 1},
        {0.55, 0.0, 0.225, 0.333},
        {0.55, 0.333, 0.225, 0.333},
        {0.55, 0.666, 0.225, 0.333},
        {0.775, 0.0, 0.225, 0.5},
        {0.775, 0.5, 0.225, 0.5},
    },
    {
        {0.0, 0.0, 0.55, 1},
        {0.55, 0.0, 0.225, 0.333},
        {0.55, 0.333, 0.225, 0.333},
        {0.55, 0.666, 0.225, 0.333},
        {0.775, 0.0, 0.225, 0.333},
        {0.775, 0.333, 0.225, 0.333},
        {0.775, 0.666, 0.225, 0.333},
    }
}

layout.set("three_columns", layout_data)
l.config.set_resize_direction(info.direction.right)
l.config.set_hidden_edges(info.direction.all)
