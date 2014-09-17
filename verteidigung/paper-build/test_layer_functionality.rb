# _________________________________ T E S T S _________________________________

require 'test/unit'
require './layer_functionality'

class TestInterface < Test::Unit::TestCase
  
  @@TESTDIR = "___TEST__DIR___"

  def teardown
    FileUtils.rm_rf @@TESTDIR
  end

  def test_check_layer_pattern
    # matches
    assert check_layer_pattern("abc-L1-3") != nil
    assert check_layer_pattern("abc-L-3") != nil
    assert check_layer_pattern("abc-L1-") != nil
    assert check_layer_pattern("abc-L1-3") != nil
    assert check_layer_pattern("abc-L1") != nil
    # non-matches
    assert check_layer_pattern("abc").nil?
    assert check_layer_pattern("abc-Lasd").nil?
    assert check_layer_pattern("abc-LE").nil?
    assert check_layer_pattern("abc-L---").nil?
  end

  def test_check_layer_pattern_multiple_specs
    assert check_layer_pattern("abc-L1_2_3") != nil
    assert check_layer_pattern("abc-L1_2-3") != nil
    assert check_layer_pattern("abc-L1-2_3") != nil
    assert check_layer_pattern("abc-L1_2-") != nil
    assert check_layer_pattern("abc-L-1_2_3") != nil
  end

end

class TestInkscapeUtils < Test::Unit::TestCase

  def teardown
    system "rm -f *__TEST__*.svg*"
  end

  def test_initialize
    utils = InkscapeUtils.new("abc-L1")
    assert_equal "abc-L1.svg", utils.filename
  end

  def test_split_name
    utils = InkscapeUtils.new("base-L-no-pattern")
    assert_equal "base-L-no-pattern.svg", utils.basename
    assert_equal "", utils.pattern
    #
    utils = InkscapeUtils.new("base-L1-3")
    assert_equal "base.svg", utils.basename
    assert_equal "1-3", utils.pattern
    #
    utils = InkscapeUtils.new("base-L1-")
    assert_equal "base.svg", utils.basename
    assert_equal "1-", utils.pattern
    #
    utils = InkscapeUtils.new("base-L-3")
    assert_equal "base.svg", utils.basename
    assert_equal "-3", utils.pattern
  end

  def test_split_name_multi_pattern
    utils = InkscapeUtils.new("base-L1_2_3")
    assert_equal "base.svg", utils.basename
    assert_equal "1_2_3", utils.pattern
    #
    utils = InkscapeUtils.new("base-L-1_2-3_4-")
    assert_equal "base.svg", utils.basename
    assert_equal "-1_2-3_4-", utils.pattern
  end

  def test_read_relevant_data_from_svg
    fn = "abc__TEST__-L1-1"
    _create_test_file("abc__TEST__.svg")
    utils = InkscapeUtils.new(fn)
    exp_layer_data = [{"label" => "Layer 1", "id" => "layer1"},
                      {"label" => "Layer2", "id" => "layer2"}]
    assert_equal exp_layer_data, utils.read_relevant_data_from_svg
  end

  def test_create_layered_svg_file_first_layer
    fn = "__TEST__.svg"
    layered_fn = "__TEST__-L1-1"
    _create_test_file fn
    utils = InkscapeUtils.new(layered_fn)
    utils.create_layered_svg_file
    dom = REXML::Document::new(File.new(layered_fn + ".svg"))
    l = []
    dom.root.each_element_with_attribute("opacity", "0.0")\
        {|e| l << e.attribute("id").value}
    assert_equal ["layer2"], l 
  end

  def test_create_layered_svg_file_both_layers
    fn = "__TEST__.svg"
    layered_fn = "__TEST__-L1-2"
    _create_test_file fn
    utils = InkscapeUtils.new(layered_fn)
    utils.create_layered_svg_file
    dom = REXML::Document::new(File.new(layered_fn + ".svg"))
    l = []
    dom.root.each_element_with_attribute("style", "display:none")\
        {|e| l << e.attribute("id").value}
    assert_equal [], l 
  end

  def test_create_layered_svg_file_second_layer
    fn = "__TEST__.svg"
    layered_fn = "__TEST__-L2-2"
    _create_test_file fn
    utils = InkscapeUtils.new(layered_fn)
    utils.create_layered_svg_file
    dom = REXML::Document::new(File.new(layered_fn + ".svg"))
    l = []
    dom.root.each_element_with_attribute("style", "display:none")\
        {|e| l << e.attribute("id").value}
    assert_equal [], l 
  end

  def _create_test_file(name)
    file = File.open(name, "w")
    file << <<-eos
  <?xml version="1.0" encoding="UTF-8" standalone="no"?>
  <svg
     xmlns:dc="http://purl.org/dc/elements/1.1/"
     xmlns:cc="http://creativecommons.org/ns#"
     xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
     xmlns:svg="http://www.w3.org/2000/svg"
     xmlns="http://www.w3.org/2000/svg"
     xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
     xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
     width="744.09448819"
     height="1052.3622047"
     id="svg2"
     version="1.1"
     inkscape:version="0.48.3.1 r9886"
     sodipodi:docname="blub.svg">
    <g
       inkscape:label="Layer 1"
       inkscape:groupmode="layer"
       id="layer1">
      <g
         sodipodi:type="inkscape:box3d"
         id="g2987"
         inkscape:perspectiveID="#perspective2985"
         inkscape:corner0="1.4183979 : 0.81626425 : 0 : 1"
         inkscape:corner7="0.34089388 : 0.34746698 : 0.25 : 1">
        <path
           sodipodi:type="inkscape:box3dside"
           id="path2999"
           style="fill:#e9e9ff;fill-rule:evenodd;stroke:none"
           inkscape:box3dsidetype="11"
           d="m 209.1408,286.01056 141.64995,-162.66624 0,294.67539 -141.64995,43.67577 z" />
        <path
           sodipodi:type="inkscape:box3dside"
           id="path2989"
           style="fill:#353564;fill-rule:evenodd;stroke:none"
           inkscape:box3dsidetype="6"
           d="m 153.84038,261.18312 0,193.84621 55.30042,6.66615 0,-175.68492 z" />
        <path
           sodipodi:type="inkscape:box3dside"
           id="path2991"
           style="fill:#4d4d9f;fill-rule:evenodd;stroke:none"
           inkscape:box3dsidetype="5"
           d="M 153.84038,261.18312 277.46211,48.238312 350.79075,123.34432 209.1408,286.01056 z" />
        <path
           sodipodi:type="inkscape:box3dside"
           id="path2997"
           style="fill:#afafde;fill-rule:evenodd;stroke:none"
           inkscape:box3dsidetype="13"
           d="M 153.84038,455.02933 277.46211,397.8538 350.79075,418.01971 209.1408,461.69548 z" />
        <path
           sodipodi:type="inkscape:box3dside"
           id="path2995"
           style="fill:#d7d7ff;fill-rule:evenodd;stroke:none"
           inkscape:box3dsidetype="14"
           d="m 277.46211,48.238312 0,349.615488 73.32864,20.16591 0,-294.67539 z" />
        <path
           sodipodi:type="inkscape:box3dside"
           id="path2993"
           style="fill:#8686bf;fill-rule:evenodd;stroke:none"
           inkscape:box3dsidetype="3"
           d="m 153.84038,261.18312 123.62173,-212.944808 0,349.615488 -123.62173,57.17553 z" />
      </g>
    </g>
    <g
       inkscape:groupmode="layer"
       id="layer2"
       inkscape:label="Layer2">
      <path
         sodipodi:type="arc"
         id="path3002"
         sodipodi:cx="502.0416"
         sodipodi:cy="200.74582"
         sodipodi:rx="35.712944"
         sodipodi:ry="46.701542"
         d="m 537.75454,200.74582 a 35.712944,46.701542 0 1 1 -71.42589,0 35.712944,46.701542 0 1 1 71.42589,0 z" />
      <path
         sodipodi:type="star"
         id="path3023"
         sodipodi:sides="5"
         sodipodi:cx="418.25352"
         sodipodi:cy="325.74112"
         sodipodi:r1="107.07057"
         sodipodi:r2="53.535287"
         sodipodi:arg1="0.31972216"
         sodipodi:arg2="0.94804069"
         inkscape:flatsided="false"
         inkscape:rounded="0"
         inkscape:randomized="0"
         d="m 519.89804,359.3937 -70.4187,9.83275 -31.82145,63.58358 -31.11208,-63.93367 -70.30495,-10.61559 51.19038,-49.34594 -11.62939,-70.14438 62.74947,33.43621 63.11758,-32.73602 -12.40907,70.01066 z"
         inkscape:transform-center-x="0.18405646"
         inkscape:transform-center-y="10.049124" />
    </g>
  </svg>
    eos
  file.close
  end

end

class TestRangeComputer < Test::Unit::TestCase
  
  def test_range_simple
    comp1 = RangeComputer.new "1-3", [nil] * 3
    assert comp1.range == [1, 2, 3]
    comp2 = RangeComputer.new "1-1", [nil]
    assert comp2.range == [1]
  end

  def test_range_single
    comp1 = RangeComputer.new "1", [nil]
    assert_equal comp1.range, [1]
    comp2 = RangeComputer.new "3", [nil, nil, nil]
    assert_equal comp2.range, [3]
  end

  def test_invalid_single_values
    comp1 = RangeComputer.new "0", [nil, nil]
    begin
      comp1.range
    rescue RangeSpecError => e
      assert e.msg.start_with? "'0' is no valid layer"
    end
    comp2 = RangeComputer.new "3", [nil, nil]
    begin
      comp2.range
    rescue RangeSpecError => e
      assert e.msg.start_with? "'3' is no valid layer"
    end
  end

  def test_range_left_bound
    comp1 = RangeComputer.new "1-", [nil] * 5
    assert comp1.range == [1, 2, 3, 4, 5]
    comp2 = RangeComputer.new "2-", [nil] * 5
    assert comp2.range == [2, 3, 4, 5]
    comp3 = RangeComputer.new "3-", []
    begin
      comp3.range
    rescue RangeSpecError => e
      assert e.msg.start_with?("Left bound too high (3)")
    end
  end

  def test_range_right_bound
    comp1 = RangeComputer.new "-3", [nil] * 3
    assert_equal [1, 2, 3], comp1.range
    comp2 = RangeComputer.new "-2", [nil] * 3
    assert comp2.range == [1, 2]
    comp3 = RangeComputer.new "-3", [nil] * 2
    begin
      comp3.range
    rescue RangeSpecError => e
      assert e.msg.start_with?("Right bound too high (3)")
    end
  end

  def test_no_range_specified
    comp1 = RangeComputer.new "", [nil] * 3
    assert_equal [], comp1.range
  end

end
