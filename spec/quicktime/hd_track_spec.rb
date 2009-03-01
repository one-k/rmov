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
      
      it "should have an audio channel map size" do
        @track.channel_map.size.should == 2
      end
      
      it "should have an audio channel map with tags" do
        @track.channel_map[0][:assignment].should == :Left
        @track.channel_map[1][:assignment].should == :Right
      end
      
      it "should have all audio tracks > 1 mono" do
        @movie.audio_tracks[1..-1].each do |tr|
          tr.channel_map[0][:assignment].should == :Mono
        end
      end

    end

  end  

  describe "SD3v2.mov audio tracks" do
    before(:each) do
      @movie = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/SD3v2.mov')
    end
    
    it "has audio track 0 with left/right" do
      track = @movie.audio_tracks[0]
      track.channel_map.should_not == nil
      track.channel_map[0][:assignment].should == :Left
      track.channel_map[1][:assignment].should == :Right
    end

    it "has audio track 1 with left/right" do
      track = @movie.audio_tracks[1]
      track.channel_map.should_not == nil
      track.channel_map[0][:assignment].should == :Left
    end
    
    it "has audio tracks with proper assignments" do
      channel_maps = @movie.audio_tracks.collect {|tr| tr.channel_map}
      channel_maps.should == [
        [{:assignment => :Left}, {:assignment => :Right}],
        [{:assignment => :Left}],
        [{:assignment => :Right}],
        [{:assignment => :Center}],
        [{:assignment => :LFEScreen}],
        [{:assignment => :LeftSurround}],
        [{:assignment => :RightSurround}],
        ]
    end

  end
  
  
  describe "SD1.mov audio tracks" do
    before(:each) do
      @movie = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/SD1.mov')
    end
    
    it "has audio tracks with proper assignments" do
      channel_maps = @movie.audio_tracks.collect {|tr| tr.channel_map}
      channel_maps.should == [
        [{:assignment => :Left}, {:assignment => :Right}],
        [{:assignment => :Mono}],
        [{:assignment => :Mono}],
        [{:assignment => :Mono}],
        [{:assignment => :Mono}],
        [{:assignment => :Mono}],
        [{:assignment => :Mono}],
        ]
    end 
  end

end