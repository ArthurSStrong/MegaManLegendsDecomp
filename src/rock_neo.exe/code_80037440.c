#include "common.h"
#include "code_80037440.h"
#include "game.h"

// Small data reached through $gp
uint16 Cell_stack_idx;
static uint8 pl_shl_count[16];

sint32 Pl_special_shot_check(void)
{
    sint32 i;
    sint32 part;
    sint32 level;
    sint32 kind;

    part                        = Player_work.weapon_right_no;
    ((sint32 *)pl_shl_count)[0] = 0;
    ((sint32 *)pl_shl_count)[1] = 0;
    ((sint32 *)pl_shl_count)[2] = 0;
    ((sint32 *)pl_shl_count)[3] = 0;
    for (i = 0; i < 16; i++)
    {
        if (Pl_shl_work_tbl[i].flags & 1)
        {
            kind = Pl_shl_work_tbl[i].kind - 0x80;
            if (kind < 16)
            {
                pl_shl_count[kind]++;
            }
        }
    }
    if (part == 6)
    {
        level = Player_work.weapon_data[6].sp_perform;
    }
    else
    {
        level = Player_work.weapon_data[part].repeat_level;
    }
    if (level >= 4)
    {
        level = 3;
    }
    return Weapon_shot_max_tbl[level][part] > pl_shl_count[part];
}

void Player_weapon_init(PL_WORK *wp)
{
    sint32 i;
    PL_PART_STATE *part;
    CODE374_VALUES *values;

    part = wp->weapon_data;
    i    = 0;
    for (; i < 16; i++, part++)
    {
        part->attack_level = 0;
        part->bullet_level = 0;
        part->dist_level   = 0;
        part->repeat_level = 0;
        part->sp_perform   = 0;
        values             = Weapon_param_tbl[i];
        part->bullet_left  = values[part->bullet_level].value1;
    }
}

void Player_weapon_refill(void)
{
    PL_WORK *wp;

    wp = &Player_work;
    wp->weapon_data[wp->part_index].bullet_left =
        Weapon_param_tbl[wp->part_index][wp->weapon_data[wp->part_index].bullet_level].value1;
}

sint32 Player_weapon_attack_calc(PL_WORK *wp, sint32 index)
{
    return Weapon_param_tbl[index][wp->weapon_data[index].attack_level].value0;
}

sint32 Player_weapon_bullet_calc(PL_WORK *wp, sint32 index)
{
    return Weapon_param_tbl[index][wp->weapon_data[index].bullet_level].value1;
}

sint32 Player_weapon_dist_calc(PL_WORK *wp, sint32 index)
{
    return Weapon_param_tbl[index][wp->weapon_data[index].dist_level].value2;
}

sint32 Player_weapon_repeat_calc(PL_WORK *wp, sint32 index)
{
    return Weapon_param_tbl[index][wp->weapon_data[index].repeat_level].value3;
}

sint32 Shot_map_check(PL_WORK *wp, sint32 amount)
{
    sint32 value;

    if (wp->value_c4 & 0xFF000000)
    {
        return 0x7FFF;
    }
    value = Scr_line_check(wp);
    if (value != 0)
    {
        if (value > 0 && (value & 0xFFFF) <= 0x7FFF && (value & CODE374_GROUP_MASK))
        {
            map_obj_damage(value & 0xFFFF, amount);
        }
        Scr_collision_check(wp);
        return value;
    }
    return 0;
}

void Map_obj_hit_damage(sint32 value, uint32 amount)
{
    if (value > 0 && (value & 0xFFFF) <= 0x7FFF && (value & CODE374_GROUP_MASK))
    {
        map_obj_damage((uint16)value, amount);
    }
}

void map_obj_damage(uint16 index, uint32 amount)
{
    CODE374_STATUS *status;
    uint32 flags;
    sint32 remaining;

    status    = &CODE374_STATUS_PTR[index];
    flags     = status->flags;
    remaining = (flags & CODE374_STATUS_MASK) - (amount >> (flags >> 12));
    if (remaining <= 0)
    {
        if (**(sint32 **)(0x8015C008 + index * 12) < 0)
        {
            map_obj_break(index);
        }
        else
        {
            map_obj_break_push(index);
        }
    }
    else
    {
        status->flags = (flags & 0xF000) | remaining;
    }
}

void map_obj_break(uint16 index)
{
    uint16 flags;
    CODE374_STATUS *status;

    status        = &CODE374_STATUS_PTR[index];
    flags         = status->flags;
    status->flags = 0;
    if ((flags >> 12) == 0)
    {
        map_obj_debris(0, 4, index);
    }
    else
    {
        map_obj_debris(4, 4, index);
    }
}

void Map_obj_remove(uint16 index)
{
    CODE374_STATUS_PTR[index].flags = 0;
}

void map_obj_debris(sint32 mode, sint32 count, uint16 index)
{
    CODE374_VALUES pos;
    CODE374_VALUES ofs;
    CODE374_OBJ *obj;
    CODE374_FX *fx;
    uint32 radius;
    uint32 height;
    uint32 r;
    sint32 a;
    uint32 i;
    uint32 n1;
    uint32 n2;
    sint32 step1;
    sint32 step2;

    obj    = (CODE374_OBJ *)CODE374_ENTRY_PTR[index].data;
    ofs    = obj->ofs;
    radius = obj->size;
    pos    = *(CODE374_VALUES *)&CODE374_ENTRY_PTR[index];
    height = radius & 0xFFFF;
    radius >>= 17;
    pos.value0 += ofs.value0;
    pos.value1 += ofs.value1 >> 1;
    pos.value2 += ofs.value2;
    Swing_Camera2(0x1E, 0x3E8, 0x1388, &pos);
    step1 = 0x1000 >> (mode - 1);
    n1    = 1 << (mode - 1);
    step2 = 0x1000 >> (count - 1);
    n2    = 1 << (count - 1);
    if (mode != 0)
    {
        for (i = 0; i < n1; i++)
        {
            fx = (CODE374_FX *)Open_EFFECT_WORK();
            if (fx != NULL)
            {
                fx->kind_0  = 5;
                fx->value_2 = 0x80;
                fx->value_3 = 2;
                r           = rand();
                a           = i * step1 - (r & 0x1F);
                if (n1 == 1)
                {
                    radius = 0;
                }
                fx->x_38 = pos.value0 + ((rcos(a) * radius) >> 12);
                fx->y_3a =
                    pos.value1 - ((((r & 0xFF) + ((r >> 7) & 0xFF) - 0x100) * (height >> 1)) >> 8);
                fx->z_3c = pos.value2 + ((rsin(a) * radius) >> 12);
            }
        }
    }
    if (count != 0)
    {
        for (i = 0; i < n2; i++)
        {
            fx = (CODE374_FX *)Open_EFFECT_WORK();
            if (fx != NULL)
            {
                fx->value_2 = 0x83;
                fx->kind_0  = 1;
                fx->value_3 = 2;
                r           = rand();
                a           = i * step2 - (r & 0x1F);
                if (n2 == 1)
                {
                    radius = 0;
                }
                fx->x_1c = pos.value0 + ((rcos(a) * radius) >> 12);
                fx->y_1e =
                    pos.value1 - ((((r & 0xFF) + ((r >> 7) & 0xFF) - 0x100) * (height >> 1)) >> 8);
                fx->z_20 = pos.value2 + ((rsin(a) * radius) >> 12);
            }
        }
    }
}

sint32 Map_obj_hit_life(sint32 value)
{
    uint32 index;
    sint32 amount;

    index = value & 0xFFFF;
    if (value > 0 && index <= 0x7FFF)
    {
        amount = CODE374_STATUS_PTR[index].flags & CODE374_STATUS_MASK;
        if (amount == 0)
        {
            return 0;
        }
        if (value & CODE374_GROUP_MASK)
        {
            return amount;
        }
    }
    return -1;
}

sint32 Map_obj_life(sint32 value)
{
    uint32 index;
    sint32 result;

    index = value & 0xFFFF;
    if (index <= 0x7FFF)
    {
        return CODE374_STATUS_PTR[index].flags & CODE374_STATUS_MASK;
    }
    return -1;
}

void map_obj_break_push(uint16 index)
{
    if (index < 0x800)
    {
        Cell_stack[Cell_stack_idx] =
            (((CODE374_ENTRY_PTR[index].x + 0x8000) >> 9) - CODE374_MAP_X) +
            ((((CODE374_ENTRY_PTR[index].z + 0x8000) >> 9) - CODE374_MAP_Z) << 8);
        Cell_stack_idx++;
    }
}

void map_obj_break_spread(uint16 cell)
{
    CODE374_VALUES unused; // never referenced; retail's frame reserves it
    uint32 x;
    uint32 z;
    uint32 nx;
    uint32 nz;
    uint32 i;
    uint16 id;
    sint16 n;

    x  = cell & 0xFF;
    z  = cell >> 8;
    id = *(CODE374_MAP + x + z * CODE374_MAP_W);
    if (CODE374_STATUS_PTR[id].flags & CODE374_STATUS_MASK)
    {
        for (i = 0; i < 8; i++)
        {
            nx = x + Cell_nbr8_x_tbl[i];
            nz = z + Cell_nbr8_z_tbl[i];
            if (nx < CODE374_MAP_W && nz < CODE374_MAP_H)
            {
                n = *(CODE374_MAP + nx + nz * CODE374_MAP_W);
                if ((uint32)(n - 1) < 0x7FF && n == id)
                {
                    Cell_stack[Cell_stack_idx] = nx + (nz << 8);
                    Cell_stack_idx++;
                }
            }
        }
        map_obj_break(id);
    }
    else
    {
        Cell_stack_idx--;
        for (i = 0; i < 4; i++)
        {
            nx = x + Cell_nbr4_x_tbl[i];
            nz = z + Cell_nbr4_z_tbl[i];
            if (nx < CODE374_MAP_W && nz < CODE374_MAP_H)
            {
                n = *(CODE374_MAP + nx + nz * CODE374_MAP_W);
                if ((uint32)(n - 1) < 0x7FF && n != id &&
                    ((uint16 *)CODE374_ENTRY_PTR[n].data)[1] ==
                        ((uint16 *)CODE374_ENTRY_PTR[id].data)[1] &&
                    (CODE374_STATUS_PTR[n].flags & CODE374_STATUS_MASK))
                {
                    Cell_stack[Cell_stack_idx] = nx + (nz << 8);
                    Cell_stack_idx++;
                }
            }
        }
    }
}

void Map_obj_break_update(void)
{
    while (Cell_stack_idx != 0)
    {
        map_obj_break_spread(Cell_stack[Cell_stack_idx - 1]);
    }
}
