require "tileutils"

local X<const> = 1
local Y<const> = 2
local WIDTH<const> = 3
local HEIGHT<const> = 4

local HORIZONTAL<const> = 0
local VERTICAL<const> = 1

local FULL_CON = {
    X = 0,
    Y = 0,
    WIDTH = 1,
    HEIGHT = 1,
}

function Is_equally_affected_by_resize_of(container, container2, d)
    local resize = false
    if d == info.direction.top then
        resize = Is_approx_equal(container2[Y], container[Y])
    elseif d == info.direction.bottom then
        resize = Is_approx_equal(container2[Y] + container2[HEIGHT], container[Y] + container[HEIGHT])
    elseif d == info.direction.left then
        resize = Is_approx_equal(container2[X], container[X])
    elseif d == info.direction.right then
        resize = Is_approx_equal(container2[X] + container2[WIDTH], container[X] + container[WIDTH])
    end
    return resize
end

-- finds containers that are affected by the container at i,j
function Get_resize_affected_containers(lt_data_el, o_lt_data_el, i, d, get_container_func, is_effected_by_func)
    local container = lt_data_el[i]
    local list = {}

    for j = 1, #lt_data_el do
        local con = lt_data_el[j]
        local alt_con = get_container_func(container, d)

        if i ~= j then
            if is_effected_by_func(o_lt_data_el[i], o_lt_data_el[j], d) then
                -- convert relative to absolute box
                local ret_con = {con[X], con[Y], con[WIDTH], con[HEIGHT], j}
                ret_con[X] = (ret_con[X]-alt_con[X])/alt_con[WIDTH]
                ret_con[Y] = (ret_con[Y]-alt_con[Y])/alt_con[HEIGHT]
                ret_con[WIDTH] = ret_con[WIDTH]/alt_con[WIDTH]
                ret_con[HEIGHT] = ret_con[HEIGHT]/alt_con[HEIGHT]
                table.insert(list, ret_con)
            end
        end
    end
    return list
end

function Move_this_container(n, d)
    -- local i = math.max(math.min(action.get_this_container_count(), #Layout_data), 1)
    -- local j = math.min(info.this_container_position(), #Layout_data[i])
    -- local container = Layout_data[i][j]
    -- Layout_data[i][j] = Move_container(container, n, d)
    -- action.arrange()
end

function Resize_this_container(n, d)
    -- local i = math.max(math.min(action.get_this_container_count(), #Layout_data), 1)
    -- local j = math.min(action.client_pos(), #Layout_data[i])
    -- Layout_data[i][j] = Resize_container(Layout_data[i][j], n, d)
    -- action.arrange()
end

--
-- returns whether container2 is affected
function Is_affected_by_resize_of(container, container2, d)
    local resize = false

    if d == info.direction.top then
        local right = Is_container_right_to(container, container2)
        local left = Is_container_left_to(container, container2)
        local container_is_higher = Is_container_over(container, container2)

        resize = container_is_higher and not (left or right)
    elseif d == info.direction.bottom then
        local right = Is_container_right_to(container, container2)
        local left = Is_container_left_to(container, container2)
        local container_is_lower = Is_container_under(container, container2)

        resize = container_is_lower and not (left or right)
    elseif d == info.direction.left then
        local over = Is_container_over(container, container2)
        local under = Is_container_under(container, container2)
        local container_is_left = Is_container_left_to(container, container2)

        resize = container_is_left and not (over or under)
    elseif d == info.direction.right then
        local over = Is_container_over(container, container2)
        local under = Is_container_under(container, container2)
        local container_is_right = Is_container_right_to(container, container2)

        resize = container_is_right and not (over or under)
    end

    return resize
end

-- val is between 0 and 1 and represents how far
local function abs_container_to_relative(con, ref_area)
    con[X] = con[X] / ref_area[WIDTH] - ref_area[X]
    con[Y] = con[Y] / ref_area[HEIGHT] - ref_area[Y]
    con[WIDTH] = con[WIDTH] / ref_area[WIDTH]
    con[HEIGHT] = con[HEIGHT] / ref_area[HEIGHT]
end

local function relative_container_to_abs(con, ref_area)
    con[X] = con[X] * ref_area[WIDTH] + ref_area[X]
    con[Y] = con[Y] * ref_area[HEIGHT] + ref_area[Y]
    con[WIDTH] = con[WIDTH] * ref_area[WIDTH]
    con[HEIGHT] = con[HEIGHT] * ref_area[HEIGHT]
end

local function create_container(x, y, width, height)
    local con = {}
    con[X] = x
    con[Y] = y
    con[WIDTH] = width
    con[HEIGHT] = height
    return con
end

local function get_area_at_zero(area)
    local x = 0
    local y = 0
    local width = area[WIDTH]
    local height = area[HEIGHT]
    local con = create_container(x, y, width, height)
    return con
end

local function get_area_local_con(con, area)
    local x1 = con[X] - area[X]
    local y1 = con[Y] - area[Y]
    local x2 = con[X] + con[WIDTH] - area[X]
    local y2 = con[Y] + con[HEIGHT] - area[Y]
    local local_con = create_container(x1, y1, x2-x1, y2-y1)
    return local_con
end

local function get_global_con(con, area)
    local x1 = con[X] + area[X]
    local y1 = con[Y] + area[Y]
    local x2 = con[X] + con[WIDTH] + area[X]
    local y2 = con[Y] + con[HEIGHT] + area[Y]
    local local_con = create_container(x1, y1, x2-x1, y2-y1)
    return local_con
end

local function assign_con_values(con, new_con)
    con[X] = new_con[X]
    con[Y] = new_con[Y]
    con[WIDTH] = new_con[WIDTH]
    con[HEIGHT] = new_con[HEIGHT]
end

local function change_base(con, old_area, new_area)
    local local_old_con = get_area_local_con(con, old_area);
    local zero_old_area = get_area_at_zero(old_area)
    local zero_new_area = get_area_at_zero(new_area)
    abs_container_to_relative(local_old_con, zero_old_area)
    relative_container_to_abs(local_old_con, zero_new_area)

    local new_con = get_global_con(local_old_con, new_area)
    assign_con_values(con, new_con)
end

local function merge_boxes(box1, box2)
    if (box1 == nil) then
        return Deep_copy(box2)
    end
    if (box2 == nil) then
        return Deep_copy(box1)
    end
    local x1 = math.min(box1[X], box2[X])
    local y1 = math.min(box1[Y], box2[Y])
    local x2 = math.max(box1[X] + box1[WIDTH], box2[X] + box2[WIDTH])
    local y2 = math.max(box1[Y] + box1[HEIGHT], box2[Y] + box2[HEIGHT])

    local con = create_container(x1, y1, x2 - x1, y2 - y1)
    return con
end

local function join_containers(con1, con2, con3)
    local res_container = merge_boxes(con1, con2)
    res_container = merge_boxes(res_container, con3)
    return res_container
end

local function intersection_of(con1, con2)
    local x1 = math.max(con1[X], con2[X])
    local y1 = math.max(con1[Y], con2[Y])
    local x2 = math.min(con1[X] + con1[WIDTH], con2[X] + con2[WIDTH])
    local y2 = math.min(con1[Y] + con1[HEIGHT], con2[Y] + con2[HEIGHT])
    local con = create_container(x1, y1, x2 - x1, y2 - y1)

    if (con[WIDTH] <= 0) then
        return nil
    end
    if (con[HEIGHT] <= 0) then
        return nil
    end
    return con
end

local function split_container(con, unaffected_area, old_alpha_area, old_beta_area)
    local non_affected_con = intersection_of(con, unaffected_area)
    local alpha_con = intersection_of(con, old_alpha_area)
    local beta_con = intersection_of(con, old_beta_area)
    return non_affected_con, alpha_con, beta_con
end

local function apply_resize(lt_data_el, old_unaffected_area, old_alpha_area, new_alpha_area, old_beta_area, new_beta_area)
    -- local i = 5
    for i = 1,#lt_data_el do
        local con = lt_data_el[i];

        local unaffected_con, alpha_con, beta_con = split_container(con, old_unaffected_area, old_alpha_area, old_beta_area);

        if (alpha_con ~= nil) then
            change_base(alpha_con, old_alpha_area, new_alpha_area)
        end
        if (beta_con ~= nil) then
            change_base(beta_con, old_beta_area, new_beta_area)
        end

        lt_data_el[i] = join_containers(unaffected_con, alpha_con, beta_con)
    end
end

local function get_cissor_container_left(alpha_area)
    local x1 = 0
    local y1 = 0
    local x2 = alpha_area[X]
    local y2 = 1
    local con = create_container(x1, y1, x2-x1, y2-y1)
    return con
end

local function get_cissor_container_top(alpha_area)
    local x1 = 0
    local y1 = 0
    local x2 = 1
    local y2 = alpha_area[Y]
    local con = create_container(x1, y1, x2-x1, y2-y1)
    return con
end

local function get_cissor_container_bottom(alpha_area)
    local x1 = 0
    local y1 = alpha_area[Y] + alpha_area[HEIGHT]
    local x2 = 1
    local y2 = 1
    local con = create_container(x1, y1, x2-x1, y2-y1)
    return con
end

local function get_cissor_container_right(alpha_area)
    local x1 = alpha_area[X] + alpha_area[WIDTH]
    local y1 = 0
    local x2 = 1
    local y2 = 1
    local con = create_container(x1, y1, x2-x1, y2-y1)
    return con
end

local function get_opposite_direction(dir)
    if dir == info.direction.left then
        return info.direction.right
    elseif dir == info.direction.right then
        return info.direction.left
    elseif dir == info.direction.top then
        return info.direction.bottom
    elseif dir == info.direction.bottom then
        return info.direction.top
    end
end

local function get_cissor_container(alpha_area, dir)
    local area = nil
    if dir == info.direction.left then
        area = get_cissor_container_left(alpha_area)
    elseif dir == info.direction.right then
        area = get_cissor_container_right(alpha_area)
    elseif dir == info.direction.top then
        area = get_cissor_container_top(alpha_area)
    elseif dir == info.direction.bottom then
        area = get_cissor_container_bottom(alpha_area)
    end
    return area
end

local function get_beta_area(alpha_area, dir)
    local area = get_cissor_container(alpha_area, dir)
    return area
end

local function get_unaffected_area(old_alpha_area, dir)
    local opposite_direction = get_opposite_direction(dir)
    local area = get_beta_area(old_alpha_area, opposite_direction)
    return area
end

local function get_alpha_container_horizontal(con)
    local x1 = con[X]
    local y1 = 0
    local x2 = con[X] + con[WIDTH]
    local y2 = 1
    local area = create_container(x1, y1, x2-x1, y2-y1)
    return area
end

local function get_alpha_container_vertical(con)
    local x1 = 0
    local y1 = con[Y]
    local x2 = 1
    local y2 = con[Y] + con[HEIGHT]
    local area = create_container(x1, y1, x2-x1, y2-y1)
    return area
end

local function get_alpha_area_from_container(con, dir)
    local area = nil
    if dir == info.direction.left or dir == info.direction.right then
        area = get_alpha_container_horizontal(con)
    elseif dir == info.direction.top or dir == info.direction.bottom then
        area = get_alpha_container_vertical(con)
    end
    return area
end

local function apply_resize_function_V2(lt_data_el, o_lt_data_el, i, n, directions)
    for x = 1,#directions do
        local dir = directions[x]

        local old_alpha_area = get_alpha_area_from_container(lt_data_el[i], dir)
        local new_alpha_area = Move_resize(old_alpha_area, 0, n, dir)
        print("n: ", n)

        local old_beta_area = get_beta_area(old_alpha_area, dir)
        local new_beta_area = get_beta_area(new_alpha_area, dir)
        local old_unaffected_area = get_unaffected_area(old_alpha_area, dir)

        apply_resize(lt_data_el, old_unaffected_area, old_alpha_area, new_alpha_area, old_beta_area, new_beta_area)
    end
end

-- TODO refactor and simplify
function Resize_all(lt_data_el, o_layout_data_el, i, n, d)
    if i > #lt_data_el then
        return lt_data_el
    end

    local directions = Get_directions(d)
    local layout_data_element = Deep_copy(lt_data_el)

    -- if Is_resize_locked(layout_data_element, o_layout_data_el, i, n, directions) then
    --     return layout_data_element
    -- end

    apply_resize_function_V2(layout_data_element, o_layout_data_el, i, n, directions)

    return layout_data_element
end

function Set(list)
    local set = {}
    for _,l in ipairs(list) do
        set[l] = true
    end
    return set
end

local function get_layout_data_element_id(o_layout_data)
    return math.max(math.min(info.get_this_container_count(), #o_layout_data), 1)
end

-- returns 0 if not found
local function get_layout_element(layout_data_element_id, resize_data)
    for j=1,#resize_data do
        local resize_data_element = resize_data[j]
        for h=1, #resize_data[j] do
            if layout_data_element_id == resize_data_element[h] then
                print("j", j)
                return j
            end
        end
    end
    return 0
end

function Resize_main_all(layout_data, o_layout_data, resize_data, n, direction)
    local layout_data_element_id = get_layout_data_element_id(o_layout_data)
    print("layout_data_element_id", layout_data_element_id)
    local layout_id = get_layout_element(layout_data_element_id, resize_data)
    print("layout_id", layout_id)
    if layout_id == 0 then
        print("is zero")
        return layout_data
    end

    local resize_element = resize_data[layout_id]
    for i=1,#resize_element do
        local id = resize_element[i]
        if id <= #o_layout_data then
            print("id: %i", id)
            -- local id = 5
            layout_data[id] = Resize_all(layout_data[id], o_layout_data[id], 1, n, direction)
        end
    end
    return layout_data
end
