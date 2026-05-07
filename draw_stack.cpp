sub draw_line(int target, int origin_x, int origin_y, float f_direction_x, float f_direction_y, int distance, int width, int color_index)
// TODO: check that f_directions are 0..1
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

sub clear(int target)
  bool on = true
  SetData(on, "Local HMI", LB, target, 1)
end sub

macro_command main()
  int DDO_ADDRESS = 90, DDO_CLEAR_ADDRESS = 90, DDO_BOX_COLOR_INDEX = 9
  int INPUT_LAYER_ADDRESS = 116
  unsigned short layer_count, rows, cols, box_width_mm, box_length_mm
  float box_heigth_mm

  GetData(layer_count, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  GetData(box_heigth_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")

  clear(DDO_CLEAR_ADDRESS)
  int DDO_WIDTH = 420, DDO_LENGTH = 480
  int COLOR_BLACK = 0, COLOR_BROWN = 9
  int STACK_ORIGIN_X, STACK_ORIGIN_Y
  int BOX_MARGIN = 5 // in px
  float SIN_30 = 0.5, SIN_60 = 0.87
  
  int box_width_px, box_length_px, box_heigth_px
  float mm_to_pixel_width_proportion, mm_to_pixel_length_proportion, mm_to_pixel_proportion

  // the idea is to calculate the magic 0..1 value to convert from mm to pixels so that the box stack perfectly fits on screen
  // you do so by dividing the entire screen's pixels by the mm length the box stack takes up
  mm_to_pixel_width_proportion = DDO_WIDTH/(SIN_60*(cols * box_width_mm + rows * box_length_mm))
  mm_to_pixel_length_proportion = (DDO_LENGTH + layer_count*BOX_MARGIN)/(SIN_30*(cols * box_width_mm + rows * box_length_mm) + layer_count * box_heigth_mm)
  // then you take the minimum proportion value, because that's the one that shrinks the most and fits the longest side to the screen
  float proportions[2]
  proportions[0] = mm_to_pixel_width_proportion
  proportions[1] = mm_to_pixel_length_proportion
  MIN(proportions[0], mm_to_pixel_proportion, 2)

  box_width_px = mm_to_pixel_proportion * box_width_mm
  box_length_px = mm_to_pixel_proportion * box_length_mm
  box_heigth_px = mm_to_pixel_proportion * box_heigth_mm
  
  
  STACK_ORIGIN_X = rows * SIN_60 * box_length_px
  STACK_ORIGIN_Y = DDO_LENGTH - box_heigth_px - (rows*box_length_px + cols*box_width_px) * SIN_30 
  // TODO: draw the palette under boxes
  int i, box_offset_px
  box_offset_px = box_heigth_px + BOX_MARGIN
  for i = 0 to layer_count-1 step 1
  // NOTE: draw_line(origin_x, origin_y, f_direction_x, f_direction_y, distance, width, color)
  // TODO: create a function that returns a list of origins for thick lines to be drawn from
  // TODO: re-do all the lines' origins taking into account that origin moved to top point of box
  
  // TODO: loop through each box in grid for each layer
  // TODO: do something like origin[0], origin[1] or middle_top_corner
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X, STACK_ORIGIN_Y - i*box_offset_px, -1*SIN_60, SIN_30, box_length_px, 0, COLOR_BLACK) // left side top stroke
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X, STACK_ORIGIN_Y - i*box_offset_px, SIN_60, SIN_30, box_width_px, 0, COLOR_BLACK) // right side top stroke
  // middle_corner[0] =//=[1]
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X + SIN_60*(box_width_px - box_length_px), STACK_ORIGIN_Y - i*box_offset_px + SIN_30*(box_length_px+box_width_px), -1*SIN_60, -1*SIN_30, box_width_px, 0, COLOR_BLACK) // left side mid stroke
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X + SIN_60*(box_width_px - box_length_px), STACK_ORIGIN_Y - i*box_offset_px + SIN_30*(box_length_px+box_width_px), SIN_60, -1*SIN_30, box_length_px, 0, COLOR_BLACK) // right side mid stroke
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X + SIN_60*(box_width_px - box_length_px), STACK_ORIGIN_Y - i*box_offset_px + SIN_30*(box_length_px+box_width_px), 0, 1, box_heigth_px, 0, COLOR_BLACK) // middle vertical stroke
  // middle_bottom_corner[0] =//=[1] 
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X + SIN_60*(box_width_px - box_length_px), STACK_ORIGIN_Y - i*box_offset_px + SIN_30*(box_length_px+box_width_px)+box_heigth_px, SIN_60, -1*SIN_30, box_length_px, 0, COLOR_BLACK) // right side bot stroke
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X + SIN_60*(box_width_px - box_length_px), STACK_ORIGIN_Y - i*box_offset_px + SIN_30*(box_length_px+box_width_px)+box_heigth_px, -1*SIN_60, -1*SIN_30, box_width_px, 0, COLOR_BLACK) // left side bot stroke
  // left_top_corner[0] =//=[1]
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X - SIN_60*box_length_px, STACK_ORIGIN_Y - i*box_offset_px + SIN_30*box_length_px, 0, 1, box_heigth_px, 0, COLOR_BLACK) // left side vertical stroke
  draw_line(DDO_ADDRESS, STACK_ORIGIN_X + SIN_60*box_width_px, STACK_ORIGIN_Y - i*box_offset_px + SIN_30*box_width_px, 0, 1, box_heigth_px, 0, COLOR_BLACK) // right side vertical stroke
  next i
  // TODO: fill top boxes top face with thick colored lines
end macro_command