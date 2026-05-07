sub draw_box(int target, int x, int y, int width, int length, bool fill, int style, int color_1, int color_2)
  unsigned short shape, shape_style, line_fill_style, primary_color, secondary_color, x1,y1,x2,y2,end_degree
  shape = 2
  if fill then
    shape_style    = 1
  else
    shape_style    = 0
  end if
  
  line_fill_style  = style
  primary_color    = color_1
  secondary_color  = color_2
  x1               = x
  y1               = y
  x2               = x + width
  y2               = y + length

  end_degree       = 0

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
  SetData(payload[0], "Local HMI", LW, target, 10)// set first point

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
  int COLOR_BLACK = 0, COLOR_BROWN = 9
  // TODO: replace temporary conveyor size with IRL size
  int DDO_WIDTH = 420, DDO_LENGTH = 480, CONVEYOR_WIDTH_mm = 1337, CONVEYOR_LENGTH_mm = 1738
  unsigned short rows, cols, box_width_mm, box_length_mm

  // GetData(destination, device_name, user-defined_tag, amount_of_destination_data_type_elements_to_read)
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  
  TRACE("%d x %d", rows, cols)
  // TODO: save rows, cols, width, length somewhere to optimize next macro call

  clear(DDO_CLEAR_ADDRESS)
  // TODO: don't draw if any of rows, cols, box_width_mm, box_length_mm == 0
  
  int box_width_px, box_length_px
  float mm_to_pixel_width_proportion, mm_to_pixel_length_proportion, f_one = 1
  mm_to_pixel_width_proportion = DDO_WIDTH / (CONVEYOR_WIDTH_mm * f_one) // TODO: check if this does what I think it does
  mm_to_pixel_length_proportion = DDO_LENGTH / (CONVEYOR_LENGTH_mm * f_one)
  box_width_px = box_width_mm * mm_to_pixel_width_proportion
  box_length_px = box_length_mm * mm_to_pixel_length_proportion

  int origin_x = 0, origin_y = 0
  origin_x = DDO_WIDTH - box_width_px * cols 

 
  // draw big fill under all the outlines
  draw_box(DDO_ADDRESS, origin_x, origin_y, box_width_px * cols, box_length_px * rows, true, 0, COLOR_BROWN, 0)
  int i, j, incremented_x, incremented_y, magic_value
  for i = 0 to rows-1 step 1
    incremented_y = origin_y + box_length_px * i
    for j = 0 to cols-1 step 1
      incremented_x = origin_x + box_width_px * j
      draw_box(DDO_ADDRESS, incremented_x, incremented_y, box_width_px, box_length_px, false, 5, 0, COLOR_BLACK)
    next j
  next i
end macro_command