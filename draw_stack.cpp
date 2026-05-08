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
  if rows * cols * box_width_mm * box_length_mm * box_heigth_mm then
    int DDO_WIDTH = 420, DDO_LENGTH = 480
    int COLOR_BLACK = 0, COLOR_BROWN = 9
    int BOX_MARGIN = 5 // in px
    float SIN_30 = 0.5, SIN_60 = 0.87
    float top_right[2], bottom_right[2], bottom_left[2], top_left[2], dwn[2], up[2]
    top_right[0] = SIN_60
    top_right[1] = -1*SIN_30
    bottom_right[0] = SIN_60
    bottom_right[1] = SIN_30
    bottom_left[0] = -1*SIN_60
    bottom_left[1] = SIN_30
    top_left[0] = -1*SIN_60
    top_left[1] = -1*SIN_30
    dwn[0] = 0.0
    dwn[1] = 1.0
    up[0] = 0.0
    up[1] = -1.0

    int box_width_px, box_length_px, box_heigth_px
    float mm_to_pixel_width_proportion, mm_to_pixel_length_proportion, mm_to_pixel_proportion
    // the idea is to calculate the magic 0..1 value to convert from mm to pixels so that the stack perfectly fits on screen
    // you do so by dividing the entire screen's pixels by the mm length the stack takes up
    mm_to_pixel_width_proportion = (DDO_WIDTH - BOX_MARGIN*(rows + cols))/(SIN_60*(cols * box_width_mm + rows * box_length_mm))
    mm_to_pixel_length_proportion = (DDO_LENGTH - BOX_MARGIN*(layer_count + rows + cols))/(SIN_30*(cols * box_width_mm + rows * box_length_mm) + layer_count * box_heigth_mm)
    // then you take the minimum proportion value, because that's the one that shrinks the most and fits the longest side to the screen
    float proportions[2]
    proportions[0] = mm_to_pixel_width_proportion
    proportions[1] = mm_to_pixel_length_proportion
    MIN(proportions[0], mm_to_pixel_proportion, 2)

    box_width_px = mm_to_pixel_proportion * box_width_mm
    box_length_px = mm_to_pixel_proportion * box_length_mm
    box_heigth_px = mm_to_pixel_proportion * box_heigth_mm
    
    int origin[2]
    origin[0] = rows * (SIN_60*box_length_px + BOX_MARGIN)
    origin[1] = DDO_LENGTH - (rows*(BOX_MARGIN + SIN_30*box_length_px) + cols*SIN_30*box_width_px + box_heigth_px)
    
    // center stack on screen when stack is fit by it's length
    if mm_to_pixel_proportion == mm_to_pixel_length_proportion then
      origin[0] = DDO_WIDTH/2 - SIN_60*(cols*box_width_px - rows*box_length_px)/2
    end if
    // TODO: draw the palette under boxes
    int middle_top_corner[2], middle_bottom_corner[2], left_top_corner[2], right_top_corner[2]
    int k, box_offset_px
    int i, j
    box_offset_px = box_heigth_px + BOX_MARGIN
    for k = 0 to layer_count-1 step 1
    // NOTE: draw_line(origin_x, origin_y, f_direction_x, f_direction_y, distance, width, color)
    // TODO: create a function that returns a list of origins for thick lines to be drawn from
      for i = 0 to rows-1 step 1
        for j = 0 to cols-1 step 1
          middle_top_corner[0] = origin[0] + SIN_60*(j*(box_width_px + BOX_MARGIN) - i*(box_length_px + BOX_MARGIN))
          middle_top_corner[1] = origin[1] + SIN_30*(j*(box_width_px + BOX_MARGIN) + i*(box_length_px + BOX_MARGIN)) - k*box_offset_px
            
          left_top_corner[0] = middle_top_corner[0] - SIN_60*box_length_px
          left_top_corner[1] = middle_top_corner[1] + SIN_30*box_length_px
          
          right_top_corner[0] = middle_top_corner[0] + SIN_60*box_width_px
          right_top_corner[1] = middle_top_corner[1] + SIN_30*box_width_px

          middle_bottom_corner[0] = left_top_corner[0] + SIN_60*box_width_px
          middle_bottom_corner[1] = left_top_corner[1] + SIN_30*box_width_px + box_heigth_px

          // TODO: loop through each box in grid for each layer
          draw_line(DDO_ADDRESS, middle_top_corner[0],    middle_top_corner[1],    bottom_left[0],  bottom_left[1],  box_length_px, 0, COLOR_BLACK) // left side top stroke
          draw_line(DDO_ADDRESS, middle_top_corner[0],    middle_top_corner[1],    bottom_right[0], bottom_right[1], box_width_px,  0, COLOR_BLACK) // right side top stroke
          draw_line(DDO_ADDRESS, left_top_corner[0],      left_top_corner[1],      bottom_right[0], bottom_right[1], box_width_px,  0, COLOR_BLACK) // left side mid stroke
          draw_line(DDO_ADDRESS, right_top_corner[0],     right_top_corner[1],     bottom_left[0],  bottom_left[1],  box_length_px, 0, COLOR_BLACK) // right side mid stroke
          draw_line(DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], up[0],           up[1],           box_heigth_px, 0, COLOR_BLACK) // middle vertical stroke
          draw_line(DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], top_right[0],    top_right[1],    box_length_px, 0, COLOR_BLACK) // right side bot stroke
          draw_line(DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], top_left[0],     top_left[1],     box_width_px,  0, COLOR_BLACK) // left side bot stroke
          draw_line(DDO_ADDRESS, left_top_corner[0],      left_top_corner[1],      dwn[0],          dwn[1],          box_heigth_px, 0, COLOR_BLACK) // left side vertical stroke
          draw_line(DDO_ADDRESS, right_top_corner[0],     right_top_corner[1],     dwn[0],          dwn[1],          box_heigth_px, 0, COLOR_BLACK) // right side vertical stroke
          next j
      next i
    next k
    // TODO: fill top boxes top face with thick colored lines
  end if
end macro_command