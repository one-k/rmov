require File.dirname(__FILE__) + '/../spec_helper.rb'

describe QuickTime::Track do
  describe "example.mov" do
    before(:each) do
      @movie = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
    end
  
    describe "video track" do
      before(:each) do
        @track = @movie.video_tracks.first
      end
    
      it "duration should be 3.1 seconds" do
        @track.duration.should == 3.1
      end
    
      it "frame count should be 31" do
        @track.frame_count.should == 31
      end
    
      it "frame rate should be 10" do
        @track.frame_rate.should == 10
      end
      
      it "should have a codec of H.264" do
        @track.codec.should == "H.264"
      end
      
      it "should have a codec of nil, if audio" do
        t = @movie.audio_tracks.first
        t.codec.should == nil
      end
      
      it "should have a width of 60" do
        @track.width.should == 60
      end
      
      it "should have a height of 50" do
        @track.height.should == 50
      end
      
      it "should be able to delete a track" do
        @track.delete
        @movie.video_tracks.should == []
      end
    
      it "should be able to add a track" do
        @track.delete
        @movie.video_tracks.should == []
      end
    
      it "should be able to disable and enable a track" do
        @track.should be_enabled
        @track.disable
        @track.should_not be_enabled
        @track.enable
        @track.should be_enabled
      end
    
      it "should have no offset" do
        @track.offset.should == 0
      end
    
      it "should be able to offset by 2.5 seconds" do
        @track.offset = 2.5
        @track.offset.should == 2.5
      end
    
      it "should be able to scale size of track" do
        @track.scale(0.5, 0.5)
        @track.bounds_width.should == 30
        @track.bounds_height.should == 25
      end
    
      it "should be able to move (translate) position of track" do
        @track.translate(10, 20)
        @track.bounds[:left].should == 10
        @track.bounds[:top].should == 20
      end
    
      it "should reset transformations" do
        @track.scale(0.5, 0.5)
        @track.translate(10, 20)
        @track.reset_transformations
        @track.bounds[:left].should == 0
        @track.bounds[:top].should == 0
        @track.bounds[:right].should == 60
        @track.bounds[:bottom].should == 50
      end
    
      it "should rotate track" do
        @track.rotate(90)
        @track.bounds_width.should == 50
        @track.bounds_height.should == 60
      end
    end
    
    describe "audio track" do
      before(:each) do
        @track = @movie.audio_tracks.first
      end
    
      it "should have a volume of 1.0" do
        @track.volume.should == 1.0
      end
    
      it "should be able to set volume to 0.5" do
        @track.volume = 0.5
        @track.volume.should == 0.5
      end
    end
  end
end
