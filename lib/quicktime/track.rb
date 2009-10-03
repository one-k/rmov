module QuickTime
  # see ext/track.c for additional methods
  class Track
    # Returns the length of this track in seconds
    # using raw_duration and time_scale.
    def duration
      raw_duration.to_f/time_scale
    end
    
    # The average frame_rate for this track. May not be exact.
    def frame_rate # what about odd frame rates such as 29.97?
      frame_count/duration
    end
    
    # Returns true/false depending on if track is an audio track.
    def audio?
      media_type == :audio
    end
    
    # Returns true/false depending on if track is a video track.
    def video?
      media_type == :video
    end
    
    # Returns true/false depending on if track is a text track.
    def text?
      media_type == :text
    end
    
    # returns numerical value for aspect ratio.  eg. 1.33333 is 4x3
    def aspect_ratio
      pix_num, pix_den = pixel_aspect_ratio
      encoded_size = encoded_pixel_dimensions
      aspect = (encoded_size[:width].to_f / encoded_size[:height].to_f) * (pix_num.to_f / pix_den.to_f)
      return :widescreen if aspect == (16.0/9.0)
      return :fullframe if aspect == (4.0/3.0)
      :other
    end

    # Returns the bounding width of this track in number of pixels.
    def bounds_width
      bounds[:right] - bounds[:left]
    end
    
    # Returns the bounding height of this track in number of pixels.
    def bounds_height
      bounds[:bottom] - bounds[:top]
    end

        
  end
end
