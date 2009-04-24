#include "rmov_ext.h"

VALUE cTrack;

static void track_free(struct RTrack *rTrack)
{
}

static void track_mark(struct RTrack *rTrack)
{
}

/*
  call-seq: new() -> track
  
  Creates a new track instance. Generally you will do this through 
  movie.tracks to fetch the Track instances for a given movie.
*/
static VALUE track_new(VALUE klass)
{
  struct RTrack *rTrack;
  return Data_Make_Struct(klass, struct RTrack, track_mark, track_free, rTrack);
}

/*
  call-seq: load(movie, index)
  
  Loads a QuickTime track from a given movie. This is done automatically 
  when calling movie.tracks.
*/
static VALUE track_load(VALUE obj, VALUE movie_obj, VALUE index_obj)
{
  RTRACK(obj)->track = GetMovieIndTrack(MOVIE(movie_obj), NUM2INT(index_obj));
  if (!RTRACK(obj)->track)
    rb_raise(eQuickTime, "Unable to fetch track for movie at index %d", NUM2INT(index_obj));
  
  return obj;
}

/*
  call-seq: raw_duration() -> duration_int
  
  Returns the raw duration of the track. Combine this with time_scale to 
  reach the duration in seconds.
*/
static VALUE track_raw_duration(VALUE obj)
{
  return INT2NUM(GetMediaDuration(TRACK_MEDIA(obj)));
}

/*
  call-seq: time_scale() -> scale_int
  
  Returns the time scale of the track. Usually only needed when working 
  with raw_duration.
*/
static VALUE track_time_scale(VALUE obj)
{
  return INT2NUM(GetMediaTimeScale(TRACK_MEDIA(obj)));
}

/*
  call-seq: frame_count() -> count
  
  Returns the number of frames in the track.
*/
static VALUE track_frame_count(VALUE obj)
{
  return INT2NUM(GetMediaSampleCount(TRACK_MEDIA(obj)));
}

/*  helper function, returns media type of the track
*/
static OSType track_get_media_type(VALUE obj)
{
  OSType media_type;
  GetMediaHandlerDescription(TRACK_MEDIA(obj), &media_type, 0, 0);  
  return media_type;
}

/*
  call-seq: media_type() -> media_type_sym
  
  Returns either :audio or :video depending on the type of track this is.
*/
static VALUE track_media_type(VALUE obj)
{
  OSType media_type = track_get_media_type(obj);
  
  if (media_type == SoundMediaType) {
    return ID2SYM(rb_intern("audio"));
  } else if (media_type == VideoMediaType) {
    return ID2SYM(rb_intern("video"));
  } else if (media_type == TextMediaType) {
    return ID2SYM(rb_intern("text"));
  } else {
    return Qnil;
  }
}

/*  returns the ImageDescriptionHandle for the track.
    If it's not a video track, return NULL
*/
static ImageDescriptionHandle track_image_description(VALUE obj)
{
  OSErr osErr;

	/* restrict reporting to video track */
  if (track_get_media_type(obj) != VideoMediaType)
    return NULL;
  
	SampleDescriptionHandle sample_description = NULL;
  sample_description = (SampleDescriptionHandle)NewHandle(sizeof(SampleDescription));
  if (LMGetMemErr() != noErr) {
    rb_raise(eQuickTime, "Memory Error %d when determining image description", LMGetMemErr());
    return NULL;
  }

	GetMediaSampleDescription(TRACK_MEDIA(obj), 1, sample_description);
	osErr = GetMoviesError();
  if (osErr != noErr) {
    rb_raise(eQuickTime, "Movie Error %d when determining image description", osErr);
    DisposeHandle((Handle)sample_description);
    return NULL;
  }
  
  return (ImageDescriptionHandle)sample_description;
}


/*
  call-seq: track_codec() -> codec_string
  
  Returns the name of the codec.  Only valid for video tracks. Others return nil.
*/
static VALUE track_codec(VALUE obj)
{
	ImageDescriptionHandle image_description = track_image_description(obj);
	
	if (image_description == NULL)
    return Qnil;

  UInt8 *codecStr = (*image_description)->name;
  
  VALUE out_str = rb_str_new( (char*)codecStr+1, (UInt8)codecStr[0] );

  DisposeHandle((Handle)image_description);

  return out_str;
}


/*
  call-seq: track_width() -> width_in_pixels
  
  Returns the width of track data.  Only valid for video tracks. Others return nil.
*/
static VALUE track_width(VALUE obj)
{
	ImageDescriptionHandle image_description = track_image_description(obj);
	
	if (image_description == NULL)
    return Qnil;

  short width = (*image_description)->width;
  
  DisposeHandle((Handle)image_description);

  return INT2NUM(width);
}

/*
  call-seq: track_height() -> height_in_pixels
  
  Returns the height of track data.  Only valid for video tracks. Others return nil.
*/
static VALUE track_height(VALUE obj)
{
	ImageDescriptionHandle image_description = track_image_description(obj);
	
	if (image_description == NULL)
    return Qnil;

  short height = (*image_description)->height;
  
  DisposeHandle((Handle)image_description);

  return INT2NUM(height);
}


/*
  call-seq: id() -> quicktime_track_id_int
  
  Returns either id number QuickTime uses to reference this track. 
  Usually only used internally.
*/
static VALUE track_id(VALUE obj)
{
  return INT2NUM(GetTrackID(TRACK(obj)));
}

/*
  call-seq: delete()
  
  Removes the track from its movie and deletes it from memory.
*/
static VALUE track_delete(VALUE obj)
{
  DisposeMovieTrack(TRACK(obj));
  return Qnil;
}

/*
  call-seq: disable()
  
  Disables the track. See enabled? to determine if it's disabled already.
*/
static VALUE track_disable(VALUE obj, VALUE boolean)
{
  SetTrackEnabled(TRACK(obj), FALSE);
  return obj;
}

/*
  call-seq: enable()
  
  Enables the track. See enabled? to determine if it's enabled already.
*/
static VALUE track_enable(VALUE obj, VALUE boolean)
{
  SetTrackEnabled(TRACK(obj), TRUE);
  return obj;
}

/*
  call-seq: enabled?() -> bool
  
  Returns true/false depending on if the track is enabled.
*/
static VALUE track_enabled(VALUE obj, VALUE boolean)
{
  if (GetTrackEnabled(TRACK(obj)) == TRUE) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

/*
  call-seq: volume() -> volume_float
  
  Returns the volume of the audio from 0.0 to 1.0.
*/
static VALUE track_get_volume(VALUE obj)
{
  return rb_float_new((double)GetTrackVolume(TRACK(obj))/0x0100);
}

/*
  call-seq: volume=(volume_float)
  
  Sets the volume to the given value (0.0 to 1.0)
*/
static VALUE track_set_volume(VALUE obj, VALUE volume_obj)
{
  SetTrackVolume(TRACK(obj), (short)(0x0100*NUM2DBL(volume_obj)));
  return Qnil;
}

/*
  call-seq: offset() -> seconds
  
  Returns the offset of the track from the beginning of the movie (in seconds).
*/
static VALUE track_get_offset(VALUE obj)
{
  return rb_float_new((double)GetTrackOffset(TRACK(obj))/GetMediaTimeScale(TRACK_MEDIA(obj)));
}

/*
  call-seq: offset=(seconds)
  
  Sets the offset of the track from the start of the movie (in seconds).
*/
static VALUE track_set_offset(VALUE obj, VALUE seconds)
{
  SetTrackOffset(TRACK(obj), TRACK_TIME(obj, seconds));
  return Qnil;
}

/*
  call-seq: new_video_media
  
  Creates a new video media for this track.
  
  Generally this method is not called directly, instead you can make a 
  new video track using Movie#new_video_track.
*/
static VALUE track_new_video_media(VALUE obj)
{
  NewTrackMedia(TRACK(obj), VideoMediaType, 600, 0, 0);
  return obj;
}

/*
  call-seq: new_audio_media
  
  Creates a new audio media for this track.
  
  Generally this method is not called directly, instead you can make a 
  new audio track using Movie#new_audio_track.
*/
static VALUE track_new_audio_media(VALUE obj)
{
  NewTrackMedia(TRACK(obj), SoundMediaType, 44100, 0, 0);
  return obj;
}

/*
  call-seq: new_text_media
  
  Creates a new text media for this track.
  
  Generally this method is not called directly, instead you can make a 
  new text track using Movie#new_text_track.
*/
static VALUE track_new_text_media(VALUE obj)
{
  NewTrackMedia(TRACK(obj), TextMediaType, 600, 0, 0);
  return obj;
}

/*  helper function to return the channel layouts
    returns layout == NULL if there is a problem
*/
static AudioChannelLayout* track_get_audio_channel_layout(VALUE obj)
{
	/* restrict reporting to audio track */
  if (track_get_media_type(obj) != SoundMediaType) return NULL;

  UInt32 size = 0;
  OSErr osErr;
  AudioChannelLayout* layout = NULL;

  osErr = QTGetTrackPropertyInfo(TRACK(obj), kQTPropertyClass_Audio, kQTAudioPropertyID_ChannelLayout, NULL, &size, NULL);
  if (osErr != noErr || size <= 0) goto bail;

  layout = (AudioChannelLayout*)calloc(1, size);
  if (layout == NULL) {
    osErr = memFullErr;
    goto bail;
  }
  
  osErr = QTGetTrackProperty(TRACK(obj), kQTPropertyClass_Audio, kQTAudioPropertyID_ChannelLayout, size, layout, NULL);
  if (osErr != noErr) goto bail;
  
  return layout;

  /* clean up in the event of an error*/
  bail:
    rb_raise(eQuickTime, "Error %d when getting audio channel layout", osErr);
    free(layout);
    return NULL;
}

/*
  call-seq: track_get_audio_channel_count() -> number_of_channels  
*/
static VALUE track_get_audio_channel_count(VALUE obj)
{
  AudioChannelLayout *layout = track_get_audio_channel_layout(obj);
  if (layout == NULL) return Qnil;

  UInt32 numChannels = (layout->mChannelLayoutTag == kAudioChannelLayoutTag_UseChannelDescriptions) ? layout->mNumberChannelDescriptions : AudioChannelLayoutTag_GetNumberOfChannels(layout->mChannelLayoutTag);
  
  free(layout);
  
  return INT2NUM(numChannels);
}

// add a channel hash with a given value(v)
#define ADD_CHANNEL(ary, c, v) c=rb_hash_new(); rb_ary_push(ary, c); rb_hash_aset(c, ID2SYM(rb_intern("assignment")), ID2SYM(rb_intern(v)))

static char* track_str_for_AudioChannelLabel(label) {
  
  char *trackStr = NULL;
  
  switch (label) {
    
    case kAudioChannelLabel_Left:
      trackStr = "Left";
      break;
    case kAudioChannelLabel_Right:
      trackStr = "Right";
      break;
    case kAudioChannelLabel_Center:
      trackStr = "Center";
      break;
    case kAudioChannelLabel_LFEScreen:
      trackStr = "LFEScreen";
      break;
    case kAudioChannelLabel_LeftSurround:
      trackStr = "LeftSurround";
      break;
    case kAudioChannelLabel_RightSurround:
      trackStr = "RightSurround";
      break;
    case kAudioChannelLabel_LeftCenter:
      trackStr = "LeftCenter";
      break;
    case kAudioChannelLabel_RightCenter:
      trackStr = "RightCenter";
      break;
    case kAudioChannelLabel_CenterSurround:
      trackStr = "CenterSurround";
      break;
    case kAudioChannelLabel_LeftSurroundDirect:
      trackStr = "LeftSurroundDirect";
      break;
    case kAudioChannelLabel_RightSurroundDirect:
      trackStr = "RightSurroundDirect";
      break;
    case kAudioChannelLabel_TopCenterSurround:
      trackStr = "TopCenterSurround";
      break;
    case kAudioChannelLabel_VerticalHeightLeft:
      trackStr = "VerticalHeightLeft";
      break;
    case kAudioChannelLabel_VerticalHeightCenter:
      trackStr = "VerticalHeightCenter";
      break;
    case kAudioChannelLabel_VerticalHeightRight:
      trackStr = "VerticalHeightRight";
      break;
    case kAudioChannelLabel_TopBackLeft:
      trackStr = "TopBackLeft";
      break;
    case kAudioChannelLabel_TopBackCenter:
      trackStr = "TopBackCenter";
      break;
    case kAudioChannelLabel_TopBackRight:
      trackStr = "TopBackRight";
      break;
    case kAudioChannelLabel_RearSurroundLeft:
      trackStr = "RearSurroundLeft";
      break;
    case kAudioChannelLabel_RearSurroundRight:
      trackStr = "RearSurroundRight";
      break;
    case kAudioChannelLabel_LeftWide:
      trackStr = "LeftWide";
      break;
    case kAudioChannelLabel_RightWide:
      trackStr = "RightWide";
      break;
    case kAudioChannelLabel_LFE2:
      trackStr = "LFE2";
      break;
    case kAudioChannelLabel_LeftTotal:
      trackStr = "LeftTotal";
      break;
    case kAudioChannelLabel_RightTotal:
      trackStr = "RightTotal";
      break;
    case kAudioChannelLabel_HearingImpaired:
      trackStr = "HearingImpaired";
      break;
    case kAudioChannelLabel_Narration:
      trackStr = "Narration";
      break;
    case kAudioChannelLabel_Mono:
      trackStr = "Mono";
      break;
    case kAudioChannelLabel_DialogCentricMix:
      trackStr = "DialogCentricMix";
      break;
    case kAudioChannelLabel_CenterSurroundDirect:
      trackStr = "CenterSurroundDirect";
      break;
  }
  
  return trackStr;
}

/*
  call-seq: track_get_audio_channel_map() -> array
    
    Returns an array n-channels in length
    Array contains Hashes in the form: {:assignment => :description} where :description is a symbol representing an audio channel description.  eg. :Left, :Right, :Mono
  
*/
static VALUE track_get_audio_channel_map(VALUE obj)
{
  AudioChannelLayout *layout = track_get_audio_channel_layout(obj);
  if (layout == NULL) return Qnil;
  
  VALUE channels = Qnil;
  UInt32 numChannels, x, highLayoutTag;
  VALUE channel;
  char message[256];
  AudioChannelLayoutTag layoutTag = layout->mChannelLayoutTag;
  
  if (layoutTag == kAudioChannelLayoutTag_UseChannelDescriptions) {
    // using the descriptions
    // not implemented
    numChannels = layout->mNumberChannelDescriptions;
    channels = rb_ary_new2(numChannels);
    
    // loop through all channels, adding assignment descriptions
    AudioChannelDescription desc;
    char *trackStr;
    for (x=0; x < numChannels; x++) {
      desc = layout->mChannelDescriptions[x];
      trackStr = track_str_for_AudioChannelLabel(desc.mChannelLabel);

      if (trackStr != NULL) {
        ADD_CHANNEL(channels, channel, trackStr);

      } else {
         // unsupported audio channel labels
         ADD_CHANNEL(channels, channel, "UnsupportedByRMov");
         sprintf(message, "ChannelLabel unsupported by rmov: %d", desc.mChannelLabel);
         rb_hash_aset(channel, ID2SYM(rb_intern("message")), rb_str_new2(message));
      }
    }
    

  } else {
    numChannels = AudioChannelLayoutTag_GetNumberOfChannels(layoutTag);
    channels = rb_ary_new2(numChannels);

    if (layoutTag == kAudioChannelLayoutTag_UseChannelBitmap) {
      // use the bitmap approach
      // not implemented
      //rb_raise(eQuickTime, "Not Implemented: kAudioChannelLayoutTag_UseChannelBitmap in track_get_audio_channel_map");
      for (x=0; x < numChannels; x++) {
        ADD_CHANNEL(channels, channel, "UnsupportedByRMov");
        rb_hash_aset(channel, ID2SYM(rb_intern("message")), rb_str_new2("UseChannelBitmap unsupported by rmov"));
      }



    } else {
      // using a standard LayoutTag
      switch (layoutTag) {

        case kAudioChannelLayoutTag_Mono:
          ADD_CHANNEL(channels, channel, "Mono");
          break;
        
        case kAudioChannelLayoutTag_Stereo:
          ADD_CHANNEL(channels, channel, "Left");
          ADD_CHANNEL(channels, channel, "Right");
          break;
          
        case kAudioChannelLayoutTag_MatrixStereo:
          ADD_CHANNEL(channels, channel, "LeftTotal");
          ADD_CHANNEL(channels, channel, "RightTotal");
          break;
          
        case kAudioChannelLayoutTag_SMPTE_DTV:
          ADD_CHANNEL(channels, channel, "Left");
          ADD_CHANNEL(channels, channel, "Right");
          ADD_CHANNEL(channels, channel, "Center");
          ADD_CHANNEL(channels, channel, "LFEScreen");
          ADD_CHANNEL(channels, channel, "LeftSurround");
          ADD_CHANNEL(channels, channel, "RightSurround");
          ADD_CHANNEL(channels, channel, "LeftTotal");
          ADD_CHANNEL(channels, channel, "RightTotal");
          break;

        default:
          // unsupported channels
          highLayoutTag = (layoutTag & 0xff0000) >> 16;
          sprintf(message, "layoutTag unsupported by rmov: (%dL << 16) | %d", highLayoutTag, numChannels);
          for (x=0; x < numChannels; x++) {
            ADD_CHANNEL(channels, channel, "UnsupportedByRMov");
            rb_hash_aset(channel, ID2SYM(rb_intern("message")), rb_str_new2(message));
          }
          //rb_raise(eQuickTime, "Unsupported ChannelLayoutTag in track_get_audio_channel_map: %d", layoutTag);
          break;
      }

    }

  }
  
  free(layout);
  
  return channels;
}

/*
  call-seq: track_encoded_pixel_dimensions() -> {:width => width, :height => height}

  returns hash of dimensions {:width => width, :height => height} of the encoded pixel
  dimensions
*/
static VALUE track_encoded_pixel_dimensions(VALUE obj)
{
  OSErr osErr = noErr;
  ImageDescriptionHandle image_description = track_image_description(obj);
  if (image_description == NULL) goto bail;
  
  SInt32 width, height;
  osErr = ICMImageDescriptionGetProperty(image_description, kQTPropertyClass_ImageDescription, kICMImageDescriptionPropertyID_EncodedWidth, sizeof(width), &width, NULL);
  if (osErr != noErr) goto bail;

  osErr = ICMImageDescriptionGetProperty(image_description, kQTPropertyClass_ImageDescription, kICMImageDescriptionPropertyID_EncodedHeight, sizeof(height), &height, NULL);
  if (osErr != noErr) goto bail;

  VALUE size_hash = rb_hash_new();

  rb_hash_aset(size_hash, ID2SYM(rb_intern("width")), INT2NUM(width));
  rb_hash_aset(size_hash, ID2SYM(rb_intern("height")), INT2NUM(height));

  DisposeHandle((Handle)image_description);
  return size_hash;

  bail:
    DisposeHandle((Handle)image_description);
    rb_raise(eQuickTime, "Error %d when getting track_encoded_pixel_dimensions", osErr);
    return Qnil;
}


/*
  call-seq: track_display_pixel_dimensions() -> {:width => width, :height => height}

  returns hash of dimensions {:width => width, :height => height} of the encoded pixel
  dimensions
*/
static VALUE track_display_pixel_dimensions(VALUE obj)
{
  OSErr osErr = noErr;
  ImageDescriptionHandle image_description = track_image_description(obj);
  if (image_description == NULL) goto bail;
  
  SInt32 width, height;
  osErr = ICMImageDescriptionGetProperty(image_description, kQTPropertyClass_ImageDescription, kICMImageDescriptionPropertyID_DisplayWidth, sizeof(width), &width, NULL);
  if (osErr != noErr) goto bail;

  osErr = ICMImageDescriptionGetProperty(image_description, kQTPropertyClass_ImageDescription, kICMImageDescriptionPropertyID_DisplayHeight, sizeof(height), &height, NULL);
  if (osErr != noErr) goto bail;

  VALUE size_hash = rb_hash_new();

  rb_hash_aset(size_hash, ID2SYM(rb_intern("width")), INT2NUM(width));
  rb_hash_aset(size_hash, ID2SYM(rb_intern("height")), INT2NUM(height));

  DisposeHandle((Handle)image_description);
  return size_hash;

  bail:
    DisposeHandle((Handle)image_description);
    rb_raise(eQuickTime, "Error %d when getting track_display_pixel_dimensions", osErr);
    return Qnil;
}



/*
  call-seq: track_pixel_aspect_ratio() -> aspect_ratio array [1, 1]
*/
static VALUE track_pixel_aspect_ratio(VALUE obj)
{
  OSErr osErr = noErr;
  ImageDescriptionHandle image_description = track_image_description(obj);
  if (image_description == NULL) goto bail;
  
  PixelAspectRatioImageDescriptionExtension dimension;
  osErr = ICMImageDescriptionGetProperty(image_description, kQTPropertyClass_ImageDescription, kICMImageDescriptionPropertyID_PixelAspectRatio, sizeof(dimension), &dimension, NULL);
  if (osErr != noErr) goto bail;
  
//  printf("\naspect ratio: %d:%d\n", dimension.hSpacing, dimension.vSpacing);
  VALUE aspect_ratio = rb_ary_new3(2, INT2NUM(dimension.hSpacing), INT2NUM(dimension.vSpacing));

  DisposeHandle((Handle)image_description);
  return aspect_ratio;

  bail:
    DisposeHandle((Handle)image_description);
    rb_raise(eQuickTime, "Error %d when getting track_encoded_pixel_dimensions", osErr);
    return Qnil;
}

void Init_quicktime_track()
{
  VALUE mQuickTime;
  mQuickTime = rb_define_module("QuickTime");
  cTrack = rb_define_class_under(mQuickTime, "Track", rb_cObject);
  rb_define_alloc_func(cTrack, track_new);
  rb_define_method(cTrack, "load_from_movie", track_load, 2);
  rb_define_method(cTrack, "raw_duration", track_raw_duration, 0);
  rb_define_method(cTrack, "time_scale", track_time_scale, 0);
  rb_define_method(cTrack, "frame_count", track_frame_count, 0);
  rb_define_method(cTrack, "media_type", track_media_type, 0);

  rb_define_method(cTrack, "codec", track_codec, 0);
  rb_define_method(cTrack, "width", track_width, 0);
  rb_define_method(cTrack, "height", track_height, 0);
  rb_define_method(cTrack, "encoded_pixel_dimensions", track_encoded_pixel_dimensions, 0);
  rb_define_method(cTrack, "display_pixel_dimensions", track_display_pixel_dimensions, 0);
  rb_define_method(cTrack, "pixel_aspect_ratio", track_pixel_aspect_ratio, 0);
  
  rb_define_method(cTrack, "channel_count", track_get_audio_channel_count, 0);
  rb_define_method(cTrack, "channel_map", track_get_audio_channel_map, 0);

  rb_define_method(cTrack, "id", track_id, 0);
  rb_define_method(cTrack, "delete", track_delete, 0);
  rb_define_method(cTrack, "enabled?", track_enabled, 0);
  rb_define_method(cTrack, "enable", track_enable, 0);
  rb_define_method(cTrack, "disable", track_disable, 0);
  rb_define_method(cTrack, "volume", track_get_volume, 0);
  rb_define_method(cTrack, "volume=", track_set_volume, 1);
  rb_define_method(cTrack, "offset", track_get_offset, 0);
  rb_define_method(cTrack, "offset=", track_set_offset, 1);
  rb_define_method(cTrack, "new_video_media", track_new_video_media, 0);
  rb_define_method(cTrack, "new_audio_media", track_new_audio_media, 0);
  rb_define_method(cTrack, "new_text_media", track_new_text_media, 0);
}
