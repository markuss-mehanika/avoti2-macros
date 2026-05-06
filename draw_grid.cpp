sub draw_box(int target, int x, int y, int width, int height, bool fill, int style, int color_1, int color_2)
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
  y2               = y + height

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

  TRACE("drawing Box(x: %d, y: %d, W: %d, H:%d)", x, y, width, height)
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
  int DDO_ADDRESS = 70, DDO_CLEAR_ADDRESS = 70, DDO_BOX_COLOR_INDEX = 9
  int INPUT_ROWS_ADDRESS = 80, INPUT_COLS_ADDRESS = 81, INPUT_WIDTH_ADDRESS = 82, INPUT_HEIGHT_ADDRESS = 83
  // TODO: replace temporary conveyor size with IRL size
  int DDO_WIDTH = 420, DDO_HEIGHT = 480, CONVEYOR_WIDTH_mm = 1337, CONVEYOR_HEIGHT_mm = 1738
  unsigned short rows, cols, width_mm, height_mm

  // GetData(destination, device_name, user-defined_tag, amount_of_destination_data_type_elements_to_read)
  GetData(rows, "Local HMI", LW, INPUT_ROWS_ADDRESS, 1)
  GetData(cols, "Local HMI", LW, INPUT_COLS_ADDRESS, 1)
  GetData(width_mm, "Local HMI", LW, INPUT_WIDTH_ADDRESS, 1)
  GetData(height_mm, "Local HMI", LW, INPUT_HEIGHT_ADDRESS, 1)
  
  TRACE("%d x %d", rows, cols)
  // TODO: save rows, cols, width, height somewhere to optimize next macro call

  clear(DDO_CLEAR_ADDRESS)
  
  int int_pixel_width, int_pixel_height
  float pixel_to_mm_width_proportion, pixel_to_mm_height_proportion, f_one = 1
  pixel_to_mm_width_proportion = DDO_WIDTH / (CONVEYOR_WIDTH_mm * f_one)
  pixel_to_mm_height_proportion = DDO_HEIGHT / (CONVEYOR_HEIGHT_mm * f_one)
  int_pixel_width = width_mm * pixel_to_mm_width_proportion
  int_pixel_height = height_mm * pixel_to_mm_height_proportion

  int origin_x, origin_y = 0
  origin_x = DDO_WIDTH - int_pixel_width * cols 
  int i, j, incremented_x, incremented_y, magic_value
  for i = 0 to rows-1 step 1
    incremented_y = origin_y + int_pixel_height * i
    for j = 0 to cols-1 step 1
      incremented_x = origin_x + int_pixel_width * j
      // TODO: draw the outlines in the loop and the background for the entire grid outside of loop
      draw_box(DDO_ADDRESS, incremented_x, incremented_y, int_pixel_width, int_pixel_height, true, 0, DDO_BOX_COLOR_INDEX, 0)
      draw_box(DDO_ADDRESS, incremented_x, incremented_y, int_pixel_width, int_pixel_height, false, 0, 0, 0)
    next j
  next i
end macro_command