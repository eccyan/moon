#
# moon/core/states/moon_player.rb
#   just a simple draft of a Music player in Moon.
#   This State is used to demonstrate various functions in the Moon engine.
#
# extension
class Input

  def self.triggered?(key_id)
    return (pressed?(key_id) || repeated?(key_id)) == 0
  end

end

# Maybe we could prefer:
#   Music.new(filename, filetype)
#   to reduce the number of extra classes floating around.
#   Music::play would still be maintained though
class BGM

  @@last = nil
  @@last_pos = 0

  def initialize(filename, filetype)
    @filename = filename
    @filetype = filetype
    @pos = 0
  end

  def filename
    return @filename
  end

  def play(pos=0)
    unless @filename.empty?
      @pos = pos
      Music.play(@filename, @filetype)
      Music.seek(@pos)
      @@last = self
      return true
    end
    return false
  end

  def resume
    play(@pos)
  end

  def pos
    return @pos
  end

  def stop
    BGM.stop
    @pos = 0
  end

  def pause
    BGM.pause
    @pos = @@last_pos
  end

  def self.playing?
    Music.playing?
  end

  def self.stopped?
    Music.stopped?
  end

  def self.finished?
    Music.finished?
  end

  def self.stop
    Music.stop
    @@last_pos = 0
  end

  def self.pause
    Music.stop
    @@last_pos = Music.pos
  end

  def self.resume
    @@last.play(@@last_pos) if @@last
  end

end

#
# the actual player container
class MoonPlayer < Container

  class Widget < Container

    attr_accessor :rx, :ry

    def initialize(rx, ry, width, height)
      @rx = rx
      @ry = ry
      super(0, 0, width, height)
    end

    def on_event(event, &func)
      @events ||= {}
      @events[event] = func
    end

    def event_list
      return @events.keys
    end

    def event?(symbol)
      return !!@events[symbol]
    end

    def call_event(symbol)
      @events[symbol].call(self) if event?(symbol)
    end

    def update
      if Input::Mouse.in_rect?(self)
        call_event(:mouse_over)
        keys = Input::Keys
        button = Input::Mouse::Buttons::LEFT
        if Input::Mouse.triggered?(button)
          if    Input::Mouse.modded?(button, keys::MOD_SHIFT)
            call_event(:mouse_focus_shift)
          elsif Input::Mouse.modded?(button, keys::MOD_CONTROL)
            call_event(:mouse_focus_control)
          elsif Input::Mouse.modded?(button, keys::MOD_ALT)
            call_event(:mouse_focus_alt)
          elsif Input::Mouse.modded?(button, keys::MOD_SUPER)
            call_event(:mouse_focus_super)
          else
            call_event(:mouse_focus_no_mod)
          end
          call_event(:mouse_focus)
        end
      end
    end

    def refresh_position(x, y)
      self.x = x + rx
      self.y = y + ry
    end

  end

  def initialize(x, y)
    # just for now, just for now
    @music = BGM.new("resources/CamelsNommingHay.ogg", "ogg")
    #@state = :stop

    @play    = Widget.new(  0,   0,  32,  32)
    @stop    = Widget.new( 34,   0,  32,  32)
    @seekbar = Widget.new( 66,  16, 128,  16)
    @seek    = Widget.new( 66,  16,  16,  16)

    @play.on_event(:mouse_focus_no_mod) do |widget|
      Music.playing? ? pause_music : play_music(true)
    end

    @play.on_event(:mouse_focus_control) do |widget|
      puts "With CONTROL"
      play_music(false)
    end

    @play.on_event(:mouse_focus_alt) do |widget|
      puts "With ALT"
    end

    @play.on_event(:mouse_focus_shift) do |widget|
      puts "With SHIFT"
      pause_music
    end

    @play.on_event(:mouse_focus_super) do |widget|
      puts "With SUPER"
    end

    #@stop.on_event(:mouse_over) {}

    @stop.on_event(:mouse_focus) do |widget|
      stop_music
    end

    @widgets = [@play, @stop, @seekbar, @seek]

    ## compare by lamba function
    comp_by = ->(ary, res, &func) do
      r = nil
      for e in ary
        r = e if (func.(e) <=> r) == res
      end
      return r
    end

    ## max_by lamba function
    max_by = ->(ary, &func) do
      return comp_by.(ary, -1, &func)
    end

    ## min_by lamba function
    min_by = ->(ary, &func) do
      return comp_by.(ary, 1, &func)
    end

    # premitive way to find the covered area of a set of Rectangles
    ## x
    wid = min_by.(@widgets) { |w| w.x }
    x = wid.x
    wid = max_by.(@widgets) { |w| w.x + w.width }
    x2 = wid.x + wid.width

    ## y
    wid = min_by.(@widgets) { |w| w.y }
    y = wid.y
    wid = max_by.(@widgets) { |w| w.y + w.height }
    y2 = wid.y + wid.height

    super(x, y, (x2 - x).abs, (y2 - y).abs)
    init_spriteset
  end

  def stop_music
    puts ">> Stopping Music"
    @music.stop
  end

  def pause_music
    @music.pause
    puts ">> Paused: #{@music.filename}"
  end

  def play_music(resume=false)
    if resume
      if @music.resume
        puts ">> Now Resuming: #{@music.filename}"
      end
    else
      if @music.play
        puts ">> Now Playing: #{@music.filename}"
      end
    end
  end

  def init_spriteset
    @spritesheet_16x16 = Spritesheet.new("resources/media_buttons_16x16.png", 16, 16)
    @spritesheet_32x32 = Spritesheet.new("resources/media_buttons_32x32.png", 32, 32)
    @spritesheet_128x16 = Spritesheet.new("resources/media_buttons_128x16.png", 128, 16)
  end

  def update_seek
    l = Music.length.to_f
    l = 1.0 if l < 1
    sx = (@seekbar.width - @seek.width) * (Music.pos / l)
    @seek.refresh_position(x + sx, y)
  end

  def render
    @spritesheet_32x32.render(@play.x, @play.y, 0, Music.playing? ? 5 : 1)
    @spritesheet_32x32.render(@stop.x, @stop.y, 0, Music.stopped? ? 4 : 0)
    @spritesheet_128x16.render(@seekbar.x, @seekbar.y, 0, Music.finished? ? 9 : 8)
    @spritesheet_16x16.render(@seek.x, @seek.y, 0, Music.playing? ? 10 : 11)
  end

  def update
    # is the mouse inside the music player
    #   just a anti-lag thing: avoids useless logic
    if Input::Mouse.in_rect?(self)
      @play.update
      @stop.update
    end
    if Input.triggered?(Input::Keys::ESCAPE)
      stop_music
    elsif Input.triggered?(Input::Keys::SPACE)
      play_music
    end
    update_seek
  end

  def on_move
    @play.refresh_position(x, y)
    @stop.refresh_position(x, y)
    @seekbar.refresh_position(x, y)
    update_seek
  end

end

#
# and finally the state
class State_MoonPlayer < State

  # TODO:
  #  optional:
  #   Sample at pos (can be used to simulate meters and such)
  def init
    @music_hud = MusicActivityHud.new(0, 0)
    @player = MoonPlayer.new(96, 128)
    puts @player.pos

    # evil stuff don't do this
    @button_hud = State_KeyHoldTest::Hud_KeyHold.new(Input::Mouse, Input::Mouse::Buttons::LEFT, 8, 8)
    @button_hud.x = 0
    @button_hud.y = @player.y + @player.height
    super
  end

  def render
    super
    @button_hud.render
    @music_hud.render
    @player.render
  end

  def update
    @player.update
    #puts Input::Mouse.pos if @ticks % 5 == 0
    super
  end

end