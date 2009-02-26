require File.dirname(__FILE__) + '/../spec_helper.rb'

describe QuickTime::Track do
  describe "HD2.mov" do
    before(:each) do
      @movie = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/HD2.mov')
    end
 
    describe "HD2.mov video track" do
      before(:each) do
        @track = @movie.video_tracks.first
      end
      
      it "should have a codec of Apple ProRes 422 (HQ)" do
        @track.codec.should == "Apple ProRes 422 (HQ)"
      end
            
    end
  
    describe "HD2.mov audio track" do 
      before(:each) do
        @track = @movie.audio_tracks.first
      end
    
      it "should have a volume of 1.0" do
        @track.volume.should == 1.0
      end
      
      it "should have a channel count" do 
        @track.channel_count.should == 2
      end
      
    end
  
  end  
  
end