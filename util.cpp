sub clear(int target)
  bool on = true
  SetData(on, "Local HMI", LB, target, 1)
end sub

sub draw_line(int target, int DDO_WIDTH, int DDO_LENGTH, int origin_x, int origin_y, float f_direction_x, float f_direction_y, int distance, int width, int color_index)
  if not 0 <= f_direction_x and not f_direction_x <= 1 then
    TRACE("WARNING: in draw_line f_direction_x not in range 0..1")
  end if
  if not 0 <= f_direction_y and not f_direction_y <= 1 then
    TRACE("WARNING: in draw_line f_direction_y not in range 0..1")
  end if
  // don't draw if line length is 0
  if distance == 0 then
    return
  end if
  int i_x1, i_y1, i_x2, i_y2
  i_x1 = origin_x
  i_y1 = origin_y
  i_x2 = origin_x + distance * f_direction_x
  i_y2 = origin_y + distance * f_direction_y
  // don't draw if both line points are outside of DDO
  if (0 > i_x1 or i_x1 > DDO_WIDTH or 0 > i_y1 or i_y1 > DDO_LENGTH) and (0 > i_x2 or i_x2 > DDO_WIDTH or 0 > i_y2 or i_y2 > DDO_LENGTH) then
    // TODO: check for the edge case where both line points are outside of DDO, but the line crosses DDO
    return
  end if
  
  unsigned short shape, shape_style, line_fill_style, primary_color, secondary_color, x1, y1, x2, y2, end_degree
  shape = 1 // lines
  shape_style = 0
  // NOTE: lines fill style controls line width in range 1..15px with values 0, 5..19 
  // width:           0    ... 15
  // line_fill_style: 0, 5 ... 19
  if not width == 0 then
    line_fill_style = width + 4
  else
    line_fill_style = 0
  end if

  primary_color = color_index
  secondary_color = 0
  x1 = origin_x
  y1 = origin_y
  x2 = origin_x + distance * f_direction_x
  y2 = origin_y + distance * f_direction_y
  end_degree = 0

  short payload[10]
    payload[0] = shape
    payload[1] = shape_style
    payload[2] = line_fill_style
    payload[3] = primary_color
    payload[4] = secondary_color
    payload[5] = x1
    payload[6] = y1
    payload[7] = x2
    payload[8] = y2
    payload[9] = end_degree

  TRACE("drawing Line(x1: %d, y1: %d, x2: %d, y2: %d)", x1, y1, x2, y2)
  SetData(payload[0], "Local HMI", LW, target, 10)

  while shape <> 0
    DELAY(1)
    GetData(shape, "Local HMI", LW, target, 1)
  wend
end sub

sub draw_box(int target, int x, int y, int width, int length, bool fill, int line_style, int color)
  unsigned short shape, shape_style, line_fill_style, primary_color, secondary_color, x1,y1,x2,y2, end_degree
  shape = 2
  if fill then
    shape_style   = 1
  else
    shape_style   = 0
  end if

  if not line_style == 0 then
    line_fill_style = line_style + 4
  else
    line_fill_style = 0
  end if

  primary_color   = color
  secondary_color = color
  x1              = x
  y1              = y
  x2              = x + width
  y2              = y + length
  end_degree      = 0

  short payload[10]
  payload[0] = shape
  payload[1] = shape_style
  payload[2] = line_fill_style
  payload[3] = primary_color
  payload[4] = secondary_color
  payload[5] = x1
  payload[6] = y1
  payload[7] = x2
  payload[8] = y2
  payload[9] = end_degree

  TRACE("drawing Box(x: %d, y: %d, W: %d, H:%d)", x, y, width, length)
  SetData(payload[0], "Local HMI", LW, target, 10)
  
  while shape <> 0
    DELAY(1)
    GetData(shape, "Local HMI", LW, target, 1)
  wend
end sub