// TODO: rename address variables to specify which memory it addresses Set/GetData(..., LW/LB, LW/LB_###)
//         ___box4width____
//        /               /|
// box   /               / |
//length3     top       1  |
//     /     plane     /   | box
//    /               /    5height
//   /_______0_______/     |
//   |               |side |
//   |               |plane|
//   |               |    /
//   8  front plane  2   /
//   |               |  6
//   |               | /
//   |_______7_______|/

// global variables
unsigned short WINDOW_ROWS, WINDOW_COLS, ROW_INDEX, COL_INDEX
unsigned short LW_DDO_ADDRESS, LB_DDO_CLEAR_ADDRESS, DDO_WIDTH, DDO_LENGTH
unsigned short LINE_COLOR, LINE_COUNTER = 0

sub init_DDO(int LW_payload_address)
  unsigned short payload[9], size = 9, zero = 0 // NOTE: make sure payload[#] and size = # match
  GetData(payload[0], "Local HMI", LW, LW_payload_address, size)
  
  WINDOW_ROWS = payload[0]
  WINDOW_COLS = payload[1]
  ROW_INDEX = payload[2]
  COL_INDEX = payload[3]
  LW_DDO_ADDRESS = payload[4]
  LB_DDO_CLEAR_ADDRESS = payload[5]
  DDO_WIDTH = payload[6]
  DDO_LENGTH = payload[7]
  LINE_COLOR = payload[8]

  // set to 0 to inform orchestrator that payload recieved and can start next async macro
  SetData(zero, "Local HMI", LW, LW_payload_address, 1)
end sub

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
  LINE_COUNTER = LINE_COUNTER + 1
end sub

sub clear(int target)
  bool on = true
  SetData(on, "Local HMI", LB, target, 1)
end sub

sub int get_line_length(int line_index, bool in_top_plane, bool in_front_plane, bool in_side_plane, int short_length, int long_length)
  Select Case line_index
    Case 0
      if not in_top_plane and not in_front_plane and in_side_plane then
        return short_length
      end if
      break
    Case 1
      if not in_top_plane and in_front_plane and not in_side_plane then
        return short_length
      end if
      break
    Case 2
      if in_top_plane and not in_front_plane and not in_side_plane then
        return short_length
      end if
      break
    Case 3
      if in_top_plane or in_front_plane then
      //^box
        if not in_top_plane and in_front_plane then
          return short_length
        end if
      else
        return 0
      end if
      break
    Case 4
      if in_top_plane or in_side_plane then
      //^box
        if not in_top_plane and in_side_plane then
          return short_length
        end if
      else
        return 0
      end if
      break
    Case 5
      if in_top_plane or in_side_plane then
      //^box
        if in_top_plane and not in_side_plane then
          return short_length
        end if
      else
        return 0
      end if
      break
    Case 6
      if in_front_plane or in_side_plane then
      //^box
        if in_front_plane and not in_side_plane then
          return short_length
        end if
      else 
        return 0
      end if
      break
    Case 7
      if in_front_plane or in_side_plane then
      //^box
        if not in_front_plane and in_side_plane then
          return short_length
        end if
      else 
        return 0
      end if
      break
    Case 8
      if in_top_plane or in_front_plane then
      //^box
        if in_top_plane and not in_front_plane then
          return short_length
        end if
      else
        return 0
      end if
      break
  Case else
      TRACE("WARNING: recieved unknown line index: %d", line_index)
      break
  end Select
  return long_length
end sub

macro_command main()
  int LW_payload_address = 1337
  init_DDO(LW_payload_address)
  unsigned short layer_count, rows, cols, box_width_mm, box_length_mm, pallet_width_mm, pallet_length_mm
  float box_heigth_mm, pallet_heigth_mm

  GetData(layer_count, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  GetData(box_heigth_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")
  GetData(pallet_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.PaletteWidth")
  GetData(pallet_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.PaletteLength")
  GetData(pallet_heigth_mm, "Local HMI", RECIPE, "Avoti_paletesana.PaletteHeight")

  clear(LB_DDO_CLEAR_ADDRESS)
  // don't draw if any value is 0
  if WINDOW_ROWS * WINDOW_COLS * layer_count * rows * cols * box_width_mm * box_length_mm * box_heigth_mm * pallet_width_mm * pallet_length_mm * pallet_heigth_mm == 0 then
    return
  end if 

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

  int lines[9]
  
  int box_width_px, box_length_px, box_heigth_px
  int pallet_width_px, pallet_length_px, pallet_heigth_px
  float mm_to_pixel_width_proportion, mm_to_pixel_length_proportion, mm_to_pixel_proportion
  int half_perimeter

  int footprint[2]
  footprint[0] = cols * box_width_mm + rows * box_length_mm
  footprint[1] = pallet_width_mm + pallet_length_mm
  MAX(footprint[0], half_perimeter, 2)

  // the idea is to calculate the magic 0..1 value to convert from mm to pixels so that the stack perfectly fits on screen
  // you do so by dividing the entire screen's pixels by the mm length the stack takes up
  mm_to_pixel_width_proportion = (WINDOW_COLS*DDO_WIDTH - BOX_MARGIN*(rows + cols))/(SIN_60*half_perimeter)
  mm_to_pixel_length_proportion = (WINDOW_ROWS*DDO_LENGTH - BOX_MARGIN*(layer_count + rows + cols))/(SIN_30*half_perimeter + layer_count * box_heigth_mm + pallet_heigth_mm)
  // then you take the minimum proportion value, because that's the one that shrinks the most and fits the longest side to the screen
  float proportions[2]
  proportions[0] = mm_to_pixel_width_proportion
  proportions[1] = mm_to_pixel_length_proportion
  MIN(proportions[0], mm_to_pixel_proportion, 2)

  box_width_px = mm_to_pixel_proportion * box_width_mm
  box_length_px = mm_to_pixel_proportion * box_length_mm
  box_heigth_px = mm_to_pixel_proportion * box_heigth_mm

  pallet_width_px = mm_to_pixel_proportion * pallet_width_mm + (cols-1)*BOX_MARGIN
  pallet_length_px = mm_to_pixel_proportion * pallet_length_mm + (rows-1)*BOX_MARGIN
  pallet_heigth_px = mm_to_pixel_proportion * pallet_heigth_mm
  
  int box_origin[2], pallet_origin[2]
  box_origin[0] = WINDOW_COLS*DDO_WIDTH/2 - COL_INDEX*DDO_WIDTH  - ROUND(SIN_60*(cols*box_width_px - rows*box_length_px)/2)
  box_origin[1] = WINDOW_ROWS*DDO_LENGTH  - ROW_INDEX*DDO_LENGTH - ROUND(rows*(BOX_MARGIN + SIN_30*box_length_px) + cols*(BOX_MARGIN + SIN_30*box_width_px) + box_heigth_px + pallet_heigth_px)

  // TODO: color box area
  int delta_width_px, delta_length_px
  int middle_top_corner[2], middle_middle_corner[2], middle_bottom_corner[2], left_top_corner[2], right_top_corner[2]
  
  TRACE("drawing Pallet(width: %d, length: %d, height: %d)", pallet_width_px, pallet_length_px, pallet_heigth_px)

  delta_width_px = cols*box_width_px + (cols-1)*BOX_MARGIN - pallet_width_px
  delta_length_px = rows*box_length_px + (rows-1)*BOX_MARGIN - pallet_length_px
  
  pallet_origin[0] = box_origin[0] + ROUND(SIN_60*(delta_width_px - delta_length_px)/2)
  pallet_origin[1] = box_origin[1] + ROUND(SIN_30*(delta_width_px + delta_length_px)/2) + box_heigth_px

  left_top_corner[0] = pallet_origin[0] - ROUND(SIN_60*pallet_length_px)
  left_top_corner[1] = pallet_origin[1] + ROUND(SIN_30*pallet_length_px)

  right_top_corner[0] = pallet_origin[0] + ROUND(SIN_60*pallet_width_px)
  right_top_corner[1] = pallet_origin[1] + ROUND(SIN_30*pallet_width_px)

  middle_bottom_corner[0] = pallet_origin[0] + ROUND(SIN_60*(pallet_width_px - pallet_length_px))
  middle_bottom_corner[1] = pallet_origin[1] + ROUND(SIN_30*(pallet_width_px + pallet_length_px) + pallet_heigth_px)

  draw_line(LW_DDO_ADDRESS, left_top_corner[0],      left_top_corner[1],      bottom_right[0], bottom_right[1], pallet_width_px,  -1, LINE_COLOR) // left side mid stroke
  draw_line(LW_DDO_ADDRESS, left_top_corner[0],      left_top_corner[1],      top_right[0],    top_right[1],    pallet_length_px, -1, LINE_COLOR) // left side top stroke
  draw_line(LW_DDO_ADDRESS, left_top_corner[0],      left_top_corner[1],      dwn[0],          dwn[1],          pallet_heigth_px, -1, LINE_COLOR) // left side vertical stroke
  draw_line(LW_DDO_ADDRESS, right_top_corner[0],     right_top_corner[1],     bottom_left[0],  bottom_left[1],  pallet_length_px, -1, LINE_COLOR) // right side mid stroke
  draw_line(LW_DDO_ADDRESS, right_top_corner[0],     right_top_corner[1],     top_left[0],     top_left[1],     pallet_width_px,  -1, LINE_COLOR) // right side top stroke
  draw_line(LW_DDO_ADDRESS, right_top_corner[0],     right_top_corner[1],     dwn[0],          dwn[1],          pallet_heigth_px, -1, LINE_COLOR) // right side vertical stroke
  draw_line(LW_DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], up[0],           up[1],           pallet_heigth_px, -1, LINE_COLOR) // middle vertical stroke
  draw_line(LW_DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], top_right[0],    top_right[1],    pallet_length_px, -1, LINE_COLOR) // right side bot stroke
  draw_line(LW_DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], top_left[0],     top_left[1],     pallet_width_px,  -1, LINE_COLOR) // left side bot stroke

  int i, j, k, inner_center_line, inner_right_line, inner_left_line
  bool in_top_plane, in_front_plane, in_side_plane
  for k = 0 to layer_count-1 step 1
  // NOTE: draw_line(origin_x, origin_y, f_direction_x, f_direction_y, distance, width, color)
  // TODO: create a function that returns a list of origins along the shortest side for thick lines to color boxes
    for i = 0 to rows-1 step 1
      for j = 0 to cols-1 step 1
        in_top_plane = k == layer_count-1
        in_front_plane = i == rows-1
        in_side_plane = j == cols-1

        if not in_top_plane and not in_front_plane and not in_side_plane then
        //^box
          // skip box
          continue
        end if

        lines[0] = get_line_length(0, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_width_px)
        lines[1] = get_line_length(1, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_length_px)
        lines[2] = get_line_length(2, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_heigth_px)
        lines[3] = get_line_length(3, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_length_px)
        lines[4] = get_line_length(4, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_width_px)
        lines[5] = get_line_length(5, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_heigth_px)
        lines[6] = get_line_length(6, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_length_px)
        lines[7] = get_line_length(7, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_width_px)
        lines[8] = get_line_length(8, in_top_plane, in_front_plane, in_side_plane, BOX_MARGIN, box_heigth_px)

        middle_top_corner[0] = box_origin[0] + ROUND(SIN_60*(j*(box_width_px + BOX_MARGIN) - i*(box_length_px + BOX_MARGIN)))
        middle_top_corner[1] = box_origin[1] + ROUND(SIN_30*(j*(box_width_px + BOX_MARGIN) + i*(box_length_px + BOX_MARGIN)) - k*(box_heigth_px + BOX_MARGIN))
          
        middle_middle_corner[0] = middle_top_corner[0] + ROUND(SIN_60*(box_width_px - box_length_px))
        middle_middle_corner[1] = middle_top_corner[1] + ROUND(SIN_30*(box_length_px + box_width_px))

        left_top_corner[0] = middle_top_corner[0] - ROUND(SIN_60*box_length_px)
        left_top_corner[1] = middle_top_corner[1] + ROUND(SIN_30*box_length_px)
        
        right_top_corner[0] = middle_top_corner[0] + ROUND(SIN_60*box_width_px)
        right_top_corner[1] = middle_top_corner[1] + ROUND(SIN_30*box_width_px)

        middle_bottom_corner[0] = left_top_corner[0] + ROUND(SIN_60*box_width_px)
        middle_bottom_corner[1] = left_top_corner[1] + ROUND(SIN_30*box_width_px + box_heigth_px)

        draw_line(LW_DDO_ADDRESS, middle_middle_corner[0], middle_middle_corner[1], top_left[0],  top_left[1],  lines[0], 0, LINE_COLOR) // left side mid stroke
        draw_line(LW_DDO_ADDRESS, middle_middle_corner[0], middle_middle_corner[1], top_right[0], top_right[1], lines[1], 0, LINE_COLOR) // right side mid stroke
        draw_line(LW_DDO_ADDRESS, middle_middle_corner[0], middle_middle_corner[1], dwn[0],       dwn[1],       lines[2], 0, LINE_COLOR) // middle vertical stroke
        draw_line(LW_DDO_ADDRESS, left_top_corner[0],      left_top_corner[1],      top_right[0], top_right[1], lines[3], 0, LINE_COLOR) // left side top stroke
        draw_line(LW_DDO_ADDRESS, right_top_corner[0],     right_top_corner[1],     top_left[0],  top_left[1],  lines[4], 0, LINE_COLOR) // right side top stroke
        draw_line(LW_DDO_ADDRESS, right_top_corner[0],     right_top_corner[1],     dwn[0],       dwn[1],       lines[5], 0, LINE_COLOR) // right side vertical stroke
        draw_line(LW_DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], top_right[0], top_right[1], lines[6], 0, LINE_COLOR) // right side bot stroke
        draw_line(LW_DDO_ADDRESS, middle_bottom_corner[0], middle_bottom_corner[1], top_left[0],  top_left[1],  lines[7], 0, LINE_COLOR) // left side bot stroke
        draw_line(LW_DDO_ADDRESS, left_top_corner[0],      left_top_corner[1],      dwn[0],       dwn[1],       lines[8], 0, LINE_COLOR) // left side vertical stroke
      next j
    next i
  next k
  TRACE("DDO at row: %d, col: %d drew %d lines", ROW_INDEX, COL_INDEX, LINE_COUNTER)
end macro_command