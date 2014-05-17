# *****************************************************************************
# *                   L A Y E R   F U N C T I O N A L I T Y                   *
# *****************************************************************************
#
# Syntax of layer files for use in includegraphics: 
#   (given the Inkscape SVG file is named <NAME>.svg)
#
#     <NAME>-L[<NUM_START_LAYER>-<NUM_END_LAYER>|<LAYER_NUM>_]+
#             [<NUM_START_LAYER>-<NUM_END_LAYER>|<LAYER_NUM>]
#
#
#     Instead of <START>-<END>, also
#
#        <START>- or -<END> can be used, in order to match everything up
#        from <START> or down from <END>, respectively.
#
# EXAMPLE: 
# ========
# Consider a file image.svg with 4 layers.
# Here are some possibilities to extract those layers in Latex:
#
#   - \includegraphics{image-L1-3}      (extracts layers 1 - 3)
#   - \includegraphics{image-L1_3_4}    (extracts layers 1, 3, and 4)
#   - \includegraphics{image-L1_3-}     (extracts layers 1, 3, and 4)
#   - \includegraphics{image-L-2_4}     (extracts layers 1, 2, and 4)
#
#
# If anything does not work as expected, please blame Sven Hager.
#
###############################################################################

require 'rexml/document'
require 'fileutils'

# _____________________________ I N T E R F A C E _____________________________

def layer_file?(filename)
  !check_layer_pattern(filename).nil?
end

$lp = /.*-L((([\d]+-[\d]+|-[\d]+|[\d]+-|[\d]+)_)*)([\d]+-[\d]+|-[\d]+|[\d]+-|[\d]+)$/
def check_layer_pattern(filename)
  return $lp.match(filename)
end

# ________________________ I M P L E M E N T A T I O N ________________________

class InkscapeUtils

  attr_reader :filename, :basename, :pattern

  def initialize(filename)
    @filename = filename + ".svg"
    match = check_layer_pattern filename
    if match.nil?
      @basename = filename + ".svg"
      @pattern = ""
    else
      @pattern = match[1] + match[4]
      index = filename.rindex @pattern
      @basename = filename[0..(index-3)] + ".svg"
    end
  end

  def read_relevant_data_from_svg
    dom = REXML::Document.new(File.new(@basename))
    layer_data = []
    dom.root.each_element_with_attribute("inkscape:groupmode", value="layer")\
        {|e| layer_data << {"id" => e.attribute("id").value,\
                            "label" => e.attribute("inkscape:label").value}}
    return layer_data
  end

  # Generates an SVG file according to the given filename and pattern.
  def create_layered_svg_file
    dom = REXML::Document::new(File.new(@basename))
    layer_data = read_relevant_data_from_svg
    patterns = @pattern.split("_")
    merged_nums = []
    for pattern in patterns
      range_computer = RangeComputer.new pattern, layer_data
      merged_nums += range_computer.range
    end
    for num in 1..layer_data.length
      if merged_nums.index(num) != nil
        next
      end
      index = num - 1
      layer_id = layer_data[index]["id"]
      dom.root.each_element_with_attribute("id", value=layer_id) {|e|
        e.add_attribute("opacity", "0.0")
      }
    end
    xml_string = dom.to_s
    new_xml_file = File.open @filename, "w"
    new_xml_file << xml_string
    new_xml_file.close
  end

end

# Objects of this class are created from a pattern string and generate
# a list of required layer numbers.
class RangeComputer
  
  @@start_end = Regexp.compile("^[0-9]+-[0-9]+$")
  @@left_bound = Regexp.compile("^[0-9]+-$")
  @@right_bound = Regexp.compile("^-[0-9]+$")
  @@single = Regexp.compile("^[0-9]+$")

  def initialize(pattern, layers)
    @pattern = pattern
    @layers = layers
    @len = layers.length
  end
  
  def range
    # try to match left and right bounds
    match = @@start_end.match(@pattern)
    if match != nil
      return _range_left_right(match)
    end
    # try to match left bound
    match = @@left_bound.match(@pattern)
    if match != nil
      return _range_left(match)
    end
    # try to match right bound
    match = @@right_bound.match(@pattern)
    if match != nil
      return _range_right(match)
    end
    # try to match single
    match = @@single.match(@pattern)
    if match != nil
      index = @pattern.to_i
      if index < 1 or index > @len
        raise RangeSpecError, "'#{index}' is no valid layer"
      end
      return [@pattern.to_i]
    end
    return []
  end

  def _range_left_right(match)
    if @pattern.index(match[0]) == 0
      arr = @pattern.split("-")
      start = arr[0].to_i
      _end = arr[1].to_i
      if start > @len or _end > @len
        raise RangeSpecError, "Incorrect layers bounds (#{start}, #{_end})"
      end
      return start.upto(_end).to_a
    end
    raise RangeSpecError, "Wrong range spec format"
  end

  def _range_left(match)
    if @pattern.index(match[0]) == 0
      start = match[0].split("-")[0].to_i
      if start > @len
        raise RangeSpecError, ("Left bound too high (#{start}), "\
            + "only #{@len} layers available")
      end
      _end = @len
      return start.upto(_end).to_a
    end
    raise RangeSpecError, "Wrong range spec format (#{@pattern})"
  end

  def _range_right(match)
    if @pattern.index(match[0]) == 0
      _end = match[0].split("-")[1].to_i
      if _end > @len
        raise RangeSpecError, ("Right bound too high (#{_end}), "\
            + "only #{@len} layers available")
      end
      start = 1
      return start.upto(_end).to_a
    end
    raise RangeSpecError, "Wrong range spec format (#{@pattern})"
  end

end

class RangeSpecError < Exception

  attr_reader :msg
  @@spec = "((A-B|A-|-B|A)_)*(A-B|A-|-B|A), (0 < A, B <= #layers)!"

  def initialize(msg)
    @msg = msg + ":\n  Expected " + @@spec
  end

  def to_s
    return @msg
  end

end

# *****************************************************************************
# *             END   L A Y E R   F U N C T I O N A L I T Y                   *
# *****************************************************************************
