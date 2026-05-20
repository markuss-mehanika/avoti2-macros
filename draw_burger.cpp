sub bool assert_non_overlaping_bits(unsigned int one, unsigned int other, int size)
  bool condition, assertion_failed
  condition = one & other == 0
  assertion_failed = not condition

  if assertion_failed then
  // get the overlapping bit's index (lowest bit if multiple)
    int i, overlap_index
    bool b_one, b_other
    for i = 0 to size-1 step 1
      GETBIT(one, b_one, i)
      GETBIT(other, b_other, i)
      if b_one and b_other then
        overlap_index = i
        break
      end if
    next i
    TRACE("assertionError: bits of %d and %d overlap at index %d", one, other, overlap_index)
  end if
  return assertion_failed
end sub

sub bool assert_less_than(int one, int other)
  bool condition, assertion_failed
  condition = one < other
  assertion_failed = not condition
    if assertion_failed then
      TRACE("assertionError: expected %d < %d", one, other)
    end if
  return assertion_failed
end sub

sub int get_highest_bit_index(unsigned int n, int size)
  if n == 0 then
    return -1
  end if
  int i, index
  bool bit
  // walk the entire size of the integer from the largest bit
  for i = 0 to size-1 step 1
    index = size-1 - i
    GETBIT(n, bit, index)
    if bit then
      return index
    end if
  next i
  TRACE("ERROR: unreachable in get_highest_bit_index")
end sub

macro_command main()
  // DDO - Dynamic Drawing Object
  int DDO_ADDRESS = 8080, LB_DDO_CLEAR_ADDRESS = 67
  int DDO_WIDTH = 650, DDO_LENGTH = 380
  int ONSCREEN_PALLET_WIDTH = 520, ONSCREEN_PALLET_LENGTH = 50
  int LW_COMMAND_ADDRESS = 420

  unsigned int is_sort_box, is_buff_box, is_rotated_90, has_spacer
  unsigned short layer_count
  // NOTE: Combined... columns are of type 32 bit unsigned
  GetData(is_sort_box, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingSortBoxBits")
  GetData(is_buff_box, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingBuffBoxBits")
  GetData(is_rotated_90, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingRot180Bits")
  GetData(has_spacer, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingSpacersBits")
  GetData(layer_count, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  int int_layer_count, size = 32
  int_layer_count = layer_count
  // NOTE: utility variable
  bool assertion_failed
  assertion_failed = assert_non_overlaping_bits(is_sort_box, is_buff_box, size)
  if assertion_failed then
    return
  end if
  assertion_failed = assert_less_than(int_layer_count, size)
  if assertion_failed then
    return
  end if

  int bits[3], top_index
  bits[0] = get_highest_bit_index(is_sort_box, size)
  bits[1] = get_highest_bit_index(is_buff_box, size)
  bits[2] = get_highest_bit_index(has_spacer, size)
  MAX(bits[0], top_index, 3)
  
  // NOTE: utility variable
  bool is_bit_on
  // NOTE: utility variable
  unsigned short command, ushort_ZERO = 0
  GetData(command, "Local HMI", LW, LW_COMMAND_ADDRESS, 1)
  // modify bits
  Select Case command
    Case 1 // add sort box
      TRACE("command \"add sort box\"")
      if top_index < layer_count-1 then
        SETBITON(is_sort_box, is_sort_box, top_index+1)
      end if
      break
    Case 2 // add buff box
      TRACE("command \"add buff box\"")
      if top_index < layer_count-1 then
        SETBITON(is_buff_box, is_buff_box, top_index+1)
      end if
      break
    Case 3 // rotate top (box)? layer
      TRACE("command \"rotate top (box)? layer\"")
      if 0 <= top_index and top_index <= size-1 then
        GETBIT(is_rotated_90, is_bit_on, top_index)
        if is_bit_on then
          SETBITOFF(is_rotated_90, is_rotated_90, top_index)
        else
          SETBITON(is_rotated_90, is_rotated_90, top_index)
        end if
      end if
      break
    Case 4 // add spacer
      TRACE("command \"add spacer\"")
      if 0 <= top_index and top_index < size-1 then
        SETBITON(has_spacer, has_spacer, top_index)
      end if
      break
    Case 5 // remove top layer
      TRACE("command \"remove top layer\"")
      if 0 <= top_index and top_index <= size-1 then
        GETBIT(has_spacer, is_bit_on, top_index)
        // remove spacer before underlying box
        if is_bit_on then
          SETBITOFF(has_spacer, has_spacer, top_index)
        else
          // no need to figure out what type of box needs to be removed
          SETBITOFF(is_sort_box, is_sort_box, top_index)
          SETBITOFF(is_buff_box, is_buff_box, top_index)
          SETBITOFF(is_rotated_90, is_rotated_90, top_index)
        end if
      end if
      break
    Case 6 // clear all
      TRACE("command \"clear all\"")
      is_sort_box = 0
      is_buff_box = 0
      is_rotated_90 = 0
      has_spacer = 0
      break
    Case else
      TRACE("WARNING: recieved unknown command: %d", command)
      break
  end Select
  SetData(ushort_ZERO, "Local HMI", LW, LW_COMMAND_ADDRESS, 1)

  // re-calculate top_index after command execution
  bits[0] = get_highest_bit_index(is_sort_box, size)
  bits[1] = get_highest_bit_index(is_buff_box, size)
  bits[2] = get_highest_bit_index(has_spacer, size)
  MAX(bits[0], top_index, 3)

  unsigned short top_layer
  top_layer = top_index + 1
  SetData(top_layer, "Local HMI", "combination_top_layer", 1)

  // calculate mm -> px
  unsigned short sort_rows, sort_cols, sort_box_width_mm, sort_box_length_mm
  unsigned short buff_rows, buff_cols, buff_box_width_mm, buff_box_length_mm
  unsigned short spacer_width_mm, spacer_length_mm
  unsigned short pallet_width_mm, pallet_length_mm
  float sort_box_height_mm, buff_box_height_mm, pallet_height_mm, spacer_height_mm

  GetData(sort_rows,          "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(sort_cols,          "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(sort_box_width_mm,  "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(sort_box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  GetData(sort_box_height_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")

  // TEMP code
  GetData(buff_rows, "Local HMI", "BufferNumberOfRows",    1)
  GetData(buff_cols, "Local HMI", "BufferNumberOfColumns", 1)
  
  GetData(buff_box_width_mm,  "Local HMI", RECIPE, "Avoti_paletesana.BufferBoxWidth")
  GetData(buff_box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.BufferBoxLength")
  GetData(buff_box_height_mm, "Local HMI", RECIPE, "Avoti_paletesana.BufferBoxHeight")
  GetData(spacer_width_mm,    "Local HMI", RECIPE, "Avoti_paletesana.SpacerWidth")
  GetData(spacer_length_mm,   "Local HMI", RECIPE, "Avoti_paletesana.SpacerLength")
  GetData(spacer_height_mm,   "Local HMI", RECIPE, "Avoti_paletesana.SpacerHeight")
  GetData(pallet_width_mm,    "Local HMI", RECIPE, "Avoti_paletesana.PaletteWidth")
  GetData(pallet_length_mm,   "Local HMI", RECIPE, "Avoti_paletesana.PaletteLength")
  GetData(pallet_height_mm,   "Local HMI", RECIPE, "Avoti_paletesana.PaletteHeight")
  
  int sort_box_width_px, sort_box_length_px, sort_box_height_px
  int buff_box_width_px, buff_box_length_px, buff_box_height_px
  int spacer_width_px, spacer_length_px, spacer_height_px
  float mm_to_pixel_width_proportion, mm_to_pixel_length_proportion, f_ONE = 1

  mm_to_pixel_width_proportion = ONSCREEN_PALLET_WIDTH / (pallet_width_mm * f_ONE)
  // TODO: boxes sometimes go above the DDO, the perfect fix is to draw the pallet instead of relying on an on-screen image
  mm_to_pixel_length_proportion = ONSCREEN_PALLET_LENGTH / (pallet_height_mm * f_ONE)

  sort_box_width_px = sort_box_width_mm * mm_to_pixel_width_proportion
  sort_box_length_px = sort_box_length_mm * mm_to_pixel_width_proportion
  sort_box_height_px = sort_box_height_mm * mm_to_pixel_length_proportion
  
  buff_box_width_px = buff_box_width_mm * mm_to_pixel_width_proportion
  buff_box_length_px = buff_box_length_mm * mm_to_pixel_width_proportion
  buff_box_height_px = buff_box_height_mm * mm_to_pixel_length_proportion

  spacer_width_px = spacer_width_mm * mm_to_pixel_width_proportion
  spacer_length_px = spacer_length_mm * mm_to_pixel_width_proportion
  spacer_height_px = spacer_height_mm * mm_to_pixel_length_proportion

  // draw
  clear(LB_DDO_CLEAR_ADDRESS)

  int origin[2]
  origin[1] = DDO_LENGTH
  
  int i, j, drawing_span[3], segment_count[2], stroke_color, fill_color
  bool FILL = true, STROKE = false
  int COLOR_BLACK = 0, COLOR_RED = 1, COLOR_GREEN = 2, COLOR_BLUE = 3
  int COLOR_BROWN = 4, COLOR_CYAN = 5, COLOR_MAGENTA = 6, COLOR_YELLOW = 7
  for i = 0 to size-1 step 1
    stroke_color = COLOR_BLACK
    fill_color = COLOR_BROWN
    drawing_span[0] = sort_box_width_px
    drawing_span[1] = buff_box_width_px
    drawing_span[2] = spacer_width_px
    segment_count[0] = sort_cols
    segment_count[1] = buff_cols
    GETBIT(is_rotated_90, is_bit_on, i)
    // change width to length and cols to rows
    if is_bit_on then
      fill_color = COLOR_YELLOW
      drawing_span[0] = sort_box_length_px
      drawing_span[1] = buff_box_length_px
      drawing_span[2] = spacer_length_px
      segment_count[0] = sort_rows 
      segment_count[1] = buff_rows 
    end if    
    
    // sort boxes
    GETBIT(is_sort_box, is_bit_on, i)
    if is_bit_on then
      origin[0] = (DDO_WIDTH - drawing_span[0]*segment_count[0])/2
      for j = 0 to segment_count[0]-1 step 1
        origin[0] = origin[0] + j*drawing_span[0]
        draw_box(DDO_ADDRESS, origin[0], origin[1] - sort_box_height_px, drawing_span[0], sort_box_height_px, FILL,   0, fill_color)
        draw_box(DDO_ADDRESS, origin[0], origin[1] - sort_box_height_px, drawing_span[0], sort_box_height_px, STROKE, 0, stroke_color)
      next j
      origin[1] = origin[1] - sort_box_height_px
    end if
    
    // buff boxes
    GETBIT(is_buff_box, is_bit_on, i)
    if is_bit_on then
      origin[0] = (DDO_WIDTH - drawing_span[1]*segment_count[1])/2
      for j = 0 to segment_count[1]-1 step 1
        origin[0] = origin[0] + j*drawing_span[1]
        draw_box(DDO_ADDRESS, origin[0], origin[1] - buff_box_height_px, drawing_span[1], buff_box_height_px, FILL,   0, fill_color)
        draw_box(DDO_ADDRESS, origin[0], origin[1] - buff_box_height_px, drawing_span[1], buff_box_height_px, STROKE, 0, stroke_color)
      next j
      origin[1] = origin[1] - buff_box_height_px
    end if
    
    // has_spacer
    GETBIT(has_spacer, is_bit_on, i)
    if is_bit_on then
      origin[0] = (DDO_WIDTH - drawing_span[2])/2
      draw_box(DDO_ADDRESS, origin[0], origin[1] - spacer_height_px, drawing_span[2], spacer_height_px, FILL,   0, COLOR_BLUE)
      draw_box(DDO_ADDRESS, origin[0], origin[1] - spacer_height_px, drawing_span[2], spacer_height_px, STROKE, 0, stroke_color)
      origin[1] = origin[1] - spacer_height_px
    end if
  next i

  SetData(is_sort_box, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingSortBoxBits")
  SetData(is_buff_box, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingBuffBoxBits")
  SetData(is_rotated_90, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingRot180Bits")
  SetData(has_spacer, "Local HMI", RECIPE, "Avoti_paletesana.CombinedPackingSpacersBits")
end macro_command