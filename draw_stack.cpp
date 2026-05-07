sub draw_line(int target, int origin_x, int origin_y, float direction_x, float direction_y, int distance, int width, int color_index)
// width:           0 1 ... 15
// line_fill_style: 0 5 ... 19
unsigned short shape, shape_style, line_fill_style, primary_color, secondary_color, x1, y1, x2, y2, end_degree

  shape = 1 // line
  shape_style = 0
  // NOTE: line fill style controls line width 5-... is width of 2-...
  if width > 0 then
    line_fill_style = width + 4
  else
    line_fill_style = 0
  end if

  primary_color = color_index
  secondary_color = 0
  x1 = origin_x
  y1 = origin_y
  x2 = origin_x + distance * direction_x
  y2 = origin_y + distance * direction_y
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

sub clear(int target)
  bool on = true
  SetData(on, "Local HMI", LB, target, 1)
end sub

macro_command main()
  int DDO_ADDRESS = 90, DDO_CLEAR_ADDRESS = 90, DDO_BOX_COLOR_INDEX = 9
  int INPUT_LAYER_ADDRESS = 116
  // TODO: change to unsigned short when creating int type pixel size variables
  int layer_count, rows, cols, box_width, box_length
  float box_heigth_mm

  GetData(layer_count, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  GetData(box_heigth_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")

  clear(DDO_CLEAR_ADDRESS)
  // TODO: replace temporary conveyor size with IRL size
  int DDO_WIDTH = 420, DDO_HEIGHT = 480, CONVEYOR_WIDTH_mm = 1344, CONVEYOR_HEIGHT_mm = 1680
  int COLOR_BLACK = 0, COLOR_BROWN = 9
  int STACK_ORIGIN_X = 10, STACK_ORIGIN_Y = 300
  float SIN_30 = 0.5, SIN_60 = 0.87

  // TODO: draw the palette under boxes
  // TODO: define mm values and convert to px
  int i, px_offset, box_heigth_px
  box_heigth_px = box_heigth_mm * 1 // NOTE: I guess that's how you cast a float to int
  px_offset = box_heigth_px + 5
  for i = 0 to layer_count-1 step 1
  // NOTE: draw_line(origin_x, origin_y, direction_x, direction_y, distance, width, color)
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X,                                     STACK_ORIGIN_Y - px_offset * i + box_heigth_px/2,                      SIN_60, SIN_30,    box_width,  box_heigth_px, COLOR_BROWN) // left side fill line
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X,                                     STACK_ORIGIN_Y - px_offset * i,                                     SIN_60, SIN_30,    box_width,  0,          COLOR_BLACK) // left side bot stroke
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X,                                     STACK_ORIGIN_Y - px_offset * i + box_heigth_px,                        SIN_60, SIN_30,    box_width,  0,          COLOR_BLACK) // left side middle stroke
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X + box_length * SIN_60,               STACK_ORIGIN_Y - px_offset * i - box_length * SIN_30,               SIN_60, SIN_30,    box_width,  0,          COLOR_BLACK) // left side top stroke
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X + box_width * SIN_60,                STACK_ORIGIN_Y - px_offset * i + box_width * SIN_30 + box_heigth_px/2, SIN_60, -1*SIN_30, box_length, box_heigth_px, COLOR_BROWN) // right side fill line
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X,                                     STACK_ORIGIN_Y - px_offset * i,                                     SIN_60, -1*SIN_30, box_length, 0,          COLOR_BLACK) // right side top stroke
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X + box_width * SIN_60,                STACK_ORIGIN_Y - px_offset * i + box_width * SIN_30,                SIN_60, -1*SIN_30, box_length, 0,          COLOR_BLACK) // right side middle stroke
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X + box_width * SIN_60,                STACK_ORIGIN_Y - px_offset * i + box_width * SIN_30 + box_heigth_px,   SIN_60, -1*SIN_30, box_length, 0,          COLOR_BLACK) // right side bot stroke
    // draw_line(DDO_ADDRESS, STACK_ORIGIN_X + box_length * SIN_60/2,             STACK_ORIGIN_Y - px_offset * i - box_length * (SIN_30/2),           SIN_60, SIN_30,    box_width,  box_length, COLOR_BROWN) // top side fill line
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X,                                     STACK_ORIGIN_Y - px_offset * i,                                     0,      1,         box_heigth_px, 0,          COLOR_BLACK) // left vertical line
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X + box_width * SIN_60,                STACK_ORIGIN_Y - px_offset * i + box_width * SIN_30,                0,      1,         box_heigth_px, 0,          COLOR_BLACK) // middle vertical line
    draw_line(DDO_ADDRESS, STACK_ORIGIN_X + (box_width + box_length) * SIN_60, STACK_ORIGIN_Y - px_offset * i + (box_width - box_length) * SIN_30, 0,      1,         box_heigth_px, 0,          COLOR_BLACK) // right vertical line
  next i
end macro_command