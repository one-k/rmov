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

/*
  call-seq: media_type() -> media_type_sym
  
  Returns either :audio or :video depending on the type of track this is.
*/
static VALUE track_media_type(VALUE obj)
{
  OSType media_type;
  
  GetMediaHandlerDescription(TRACK_MEDIA(obj), &media_type, 0, 0);
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
	Media track_media = TRACK_MEDIA(obj);
  OSType media_type;
  OSErr osErr = noErr;

	/* restrict reporting to video track */
  GetMediaHandlerDescription(track_media, &media_type, 0, 0);
  if (media_type != VideoMediaType)
    return NULL;
  
	SampleDescriptionHandle sample_description = NULL;
  sample_description = (SampleDescriptionHandle)NewHandle(sizeof(SampleDescription));
  if (LMGetMemErr() != noErr) {
    rb_raise(eQuickTime, "Memory Error %d when determining image description", LMGetMemErr());
    return NULL;
  }

	GetMediaSampleDescription(track_media, 1, sample_description);
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


/*  returns the AudioStreamBasicDescription for the track.
    If it's not an audio track, return NULL
*/
static SoundDescriptionHandle track_audio_description(VALUE obj)
{
	Media track_media = TRACK_MEDIA(obj);
  OSType media_type;
  OSErr osErr = noErr;

	/* restrict reporting to audio track */
  GetMediaHandlerDescription(track_media, &media_type, 0, 0);
  if (media_type != SoundMediaType)
    return NULL;
  
	SoundDescriptionHandle sample_description = NULL;
  sample_description = (SoundDescriptionHandle)NewHandle(sizeof(SampleDescription));
  if (LMGetMemErr() != noErr) {
    rb_raise(eQuickTime, "Memory Error %d when determining audio description", LMGetMemErr());
    return NULL;
  }

	GetMediaSampleDescription(track_media, 1, (SampleDescriptionHandle)sample_description);
	osErr = GetMoviesError();
  if (osErr != noErr) {
    rb_raise(eQuickTime, "GetMediaSampleDescription Error %d when determining audio description", osErr);
    DisposeHandle((Handle)sample_description);
    return NULL;
  }

  return sample_description;
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
