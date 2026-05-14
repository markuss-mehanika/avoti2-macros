// global variables
int DDO_WIDTH = 420, DDO_LENGTH = 480

sub draw_line(int target, int origin_x, int origin_y, float f_direction_x, float f_direction_y, int distance, int width, int color_index)
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
  unsigned short shape, shape_style, line_fill_style, primary_color, secondary_color, x1,y1,x2,y2,end_degree
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
  SetData(payload[0], "Local HMI", LW, target, 10) // set first point

  while shape <> 0
    DELAY(1)
    GetData(shape, "Local HMI", LW, target, 1)
  wend
end sub

sub clear(int target)
  bool on = true
  SetData(on, "Local HMI", LB, target, 1)
end sub

macro_command main()
  // DDO - Dynamic Drawing Object
  int DDO_ADDRESS = 70, DDO_CLEAR_ADDRESS = 70
  // TODO: replace temporary conveyor size with IRL size
  int CONVEYOR_WIDTH_mm = 1337, CONVEYOR_LENGTH_mm = 1738

  int COLOR_BLACK = 0, COLOR_BROWN = 9
  bool FILL = true, STROKE = false
  
  unsigned short rows, cols, box_width_mm, box_length_mm
  // GetData(destination, device_name, user-defined_tag, amount_of_destination_data_type_elements_to_read)
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  
  TRACE("%d x %d", rows, cols)
  clear(DDO_CLEAR_ADDRESS)

  // don't draw if any value is 0
  if rows * cols * box_width_mm * box_length_mm == 0 then
    return
  end if
  
  int box_width_px, box_length_px
  float mm_to_pixel_width_proportion, mm_to_pixel_length_proportion, f_ONE = 1
  mm_to_pixel_width_proportion = DDO_WIDTH / (CONVEYOR_WIDTH_mm * f_ONE)
  mm_to_pixel_length_proportion = DDO_LENGTH / (CONVEYOR_LENGTH_mm * f_ONE)
  box_width_px = box_width_mm * mm_to_pixel_width_proportion
  box_length_px = box_length_mm * mm_to_pixel_length_proportion

  int origin_x = 0, origin_y = 0
  origin_x = DDO_WIDTH - box_width_px * cols 

  float dwn[2], right[2]
  dwn[0] = 0
  dwn[1] = 1
  right[0] = 1
  right[1] = 0
  // draw big fill and stroke first
  draw_box(DDO_ADDRESS, origin_x, origin_y, box_width_px * cols, box_length_px * rows, FILL, 1, COLOR_BROWN)
  draw_box(DDO_ADDRESS, origin_x, origin_y, box_width_px * cols, box_length_px * rows, STROKE, 1, COLOR_BLACK)
  int i, j, incremented_x, incremented_y
  // draw rows-1 thin vertical lines
  for j = 1 to cols-1 step 1
    draw_line(DDO_ADDRESS, origin_x + box_width_px * j, origin_y, dwn[0], dwn[1], box_length_px*rows, 0, COLOR_BLACK)
  next j
  // draw rows-1 thin horizontal lines
  for i = 1 to rows-1 step 1
  draw_line(DDO_ADDRESS, origin_x, origin_y + box_length_px * i, right[0], right[1], box_width_px*cols, 0, COLOR_BLACK)
  next i
end macro_command