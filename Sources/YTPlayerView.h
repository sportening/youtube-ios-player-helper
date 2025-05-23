// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

@class YTPlayerView;

/** These enums represent the state of the current video in the player. */
typedef NS_ENUM(NSInteger, YTPlayerState) {
    kYTPlayerStateUnstarted,
    kYTPlayerStateEnded,
    kYTPlayerStatePlaying,
    kYTPlayerStatePaused,
    kYTPlayerStateBuffering,
    kYTPlayerStateCued,
    kYTPlayerStateUnknown
};

/** These enums represent the resolution of the currently loaded video. */
typedef NS_ENUM(NSInteger, YTPlaybackQuality) {
    kYTPlaybackQualitySmall,
    kYTPlaybackQualityMedium,
    kYTPlaybackQualityLarge,
    kYTPlaybackQualityHD720,
    kYTPlaybackQualityHD1080,
    kYTPlaybackQualityHighRes,
    kYTPlaybackQualityAuto, /** Addition for YouTube Live Events. */
    kYTPlaybackQualityDefault,
    kYTPlaybackQualityUnknown /** This should never be returned. It is here for future proofing. */
};

/** These enums represent error codes thrown by the player. */
typedef NS_ENUM(NSInteger, YTPlayerError) {
    kYTPlayerErrorInvalidParam,
    kYTPlayerErrorHTML5Error,
    kYTPlayerErrorVideoNotFound, // Functionally equivalent error codes 100 and
    // 105 have been collapsed into |kYTPlayerErrorVideoNotFound|.
    kYTPlayerErrorNotEmbeddable, // Functionally equivalent error codes 101 and
    // 150 have been collapsed into |kYTPlayerErrorNotEmbeddable|.
    kYTPlayerErrorUnknown
};

/** Completion handlers for player API calls. */
typedef void (^YTIntCompletionHandler)(int result, NSError *_Nullable error);
typedef void (^YTFloatCompletionHandler)(float result, NSError *_Nullable error);
typedef void (^YTDoubleCompletionHandler)(double result, NSError *_Nullable error);
typedef void (^YTStringCompletionHandler)(NSString *_Nullable result, NSError *_Nullable error);
typedef void (^YTArrayCompletionHandler)(NSArray *_Nullable result, NSError *_Nullable error);
typedef void (^YTURLCompletionHandler)(NSURL *_Nullable result, NSError *_Nullable error);
typedef void (^YTPlayerStateCompletionHandler)(YTPlayerState result, NSError *_Nullable error);
typedef void (^YTPlaybackQualityCompletionHandler)(YTPlaybackQuality result,
                                                   NSError *_Nullable error);

/**
 * A delegate for ViewControllers to respond to YouTube player events outside
 * of the view, such as changes to video playback state or playback errors.
 * The callback functions correlate to the events fired by the IFrame API.
 * For the full documentation, see the IFrame documentation here:
 *     https://developers.google.com/youtube/iframe_api_reference#Events
 */
@protocol YTPlayerViewDelegate<NSObject>

@optional
/**
 * Invoked when the player view is ready to receive API calls.
 *
 * @param playerView The YTPlayerView instance that has become ready.
 */
- (void)playerViewDidBecomeReady:(nonnull YTPlayerView *)playerView;

/**
 * Callback invoked when player state has changed, e.g. stopped or started playback.
 *
 * @param playerView The YTPlayerView instance where playback state has changed.
 * @param state YTPlayerState designating the new playback state.
 */
- (void)playerView:(nonnull YTPlayerView *)playerView didChangeToState:(YTPlayerState)state;

/**
 * Callback invoked when playback quality has changed.
 *
 * @param playerView The YTPlayerView instance where playback quality has changed.
 * @param quality YTPlaybackQuality designating the new playback quality.
 */
- (void)playerView:(nonnull YTPlayerView *)playerView didChangeToQuality:(YTPlaybackQuality)quality;

/**
 * Callback invoked when an error has occured.
 *
 * @param playerView The YTPlayerView instance where the error has occurred.
 * @param error YTPlayerError containing the error state.
 */
- (void)playerView:(nonnull YTPlayerView *)playerView receivedError:(YTPlayerError)error;

/**
 * Callback invoked frequently when playBack is plaing.
 *
 * @param playerView The YTPlayerView instance where the error has occurred.
 * @param playTime float containing curretn playback time.
 */
- (void)playerView:(nonnull YTPlayerView *)playerView didPlayTime:(float)playTime;

/**
 * Callback invoked when setting up the webview to allow custom colours so it fits in
 * with app color schemes. If a transparent view is required specify clearColor and
 * the code will handle the opacity etc.
 * 
 * @param playerView The YTPlayerView instance where the error has occurred.
 * @return A color object that represents the background color of the webview.
 */
- (nonnull UIColor *)playerViewPreferredWebViewBackgroundColor:(nonnull YTPlayerView *)playerView;

/**
 * Callback invoked when initially loading the YouTube iframe to the webview to display a custom
 * loading view while the player view is not ready. This loading view will be dismissed just before
 * -playerViewDidBecomeReady: callback is invoked. The loading view will be automatically resized
 * to cover the entire player view.
 *
 * The default implementation does not display any custom loading views so the player will display
 * a blank view with a background color of (-playerViewPreferredWebViewBackgroundColor:).
 * 
 * Note that the custom loading view WILL NOT be displayed after iframe is loaded. It will be 
 * handled by YouTube iframe API. This callback is just intended to tell users the view is actually
 * doing something while iframe is being loaded, which will take some time if users are in poor networks.
 *
 * @param playerView The YTPlayerView instance where the error has occurred.
 * @return A view object that will be displayed while YouTube iframe API is being loaded. 
 *         Pass nil to display no custom loading view. Default implementation returns nil.
 */
- (nullable UIView *)playerViewPreferredInitialLoadingView:(nonnull YTPlayerView *)playerView;

@end

/**
 * YTPlayerView is a custom UIView that client developers will use to include YouTube
 * videos in their iOS applications. It can be instantiated programmatically, or via
 * Interface Builder. Use the methods YTPlayerView::loadWithVideoId:,
 * YTPlayerView::loadWithPlaylistId: or their variants to set the video or playlist
 * to populate the view with.
 */
@interface YTPlayerView : UIView

@property(nonatomic, nullable, readonly) WKWebView *webView;

/** A delegate to be notified on playback events. */
@property(nonatomic, weak, nullable) id<YTPlayerViewDelegate> delegate;

- (nonnull instancetype)initWithOriginURL:(nonnull NSURL*)originURL;

/**
 * This method loads the player with the given video ID.
 * This is a convenience method for calling YTPlayerView::loadPlayerWithVideoId:withPlayerVars:
 * without player variables.
 *
 * This method reloads the entire contents of the webview and regenerates its HTML contents.
 * To change the currently loaded video without reloading the entire webview, use the
 * YTPlayerView::cueVideoById:startSeconds: family of methods.
 *
 * @param videoId The YouTube video ID of the video to load in the player view.
 * @return YES if player has been configured correctly, NO otherwise.
 */
- (BOOL)loadWithVideoId:(nonnull NSString *)videoId;

/**
 * This method loads the player with the given playlist ID.
 * This is a convenience method for calling YTPlayerView::loadWithPlaylistId:withPlayerVars:
 * without player variables.
 *
 * This method reloads the entire contents of the webview and regenerates its HTML contents.
 * To change the currently loaded video without reloading the entire webview, use the
 * YTPlayerView::cuePlaylistByPlaylistId:index:startSeconds:
 * family of methods.
 *
 * @param playlistId The YouTube playlist ID of the playlist to load in the player view.
 * @return YES if player has been configured correctly, NO otherwise.
 */
- (BOOL)loadWithPlaylistId:(nonnull NSString *)playlistId;

/**
 * This method loads the player with the given video ID and player variables. Player variables
 * specify optional parameters for video playback. For instance, to play a YouTube
 * video inline, the following playerVars dictionary would be used:
 *
 * @code
 * @{ @"playsinline" : @1 };
 * @endcode
 *
 * Note that when the documentation specifies a valid value as a number (typically 0, 1 or 2),
 * both strings and integers are valid values. The full list of parameters is defined at:
 *   https://developers.google.com/youtube/player_parameters?playerVersion=HTML5.
 *
 * This method reloads the entire contents of the webview and regenerates its HTML contents.
 * To change the currently loaded video without reloading the entire webview, use the
 * YTPlayerView::cueVideoById:startSeconds: family of methods.
 *
 * @param videoId The YouTube video ID of the video to load in the player view.
 * @param playerVars An NSDictionary of player parameters.
 * @return YES if player has been configured correctly, NO otherwise.
 */
- (BOOL)loadWithVideoId:(nonnull NSString *)videoId playerVars:(nullable NSDictionary *)playerVars;

/**
 * This method loads the player with the given playlist ID and player variables. Player variables
 * specify optional parameters for video playback. For instance, to play a YouTube
 * video inline, the following playerVars dictionary would be used:
 *
 * @code
 * @{ @"playsinline" : @1 };
 * @endcode
 *
 * Note that when the documentation specifies a valid value as a number (typically 0, 1 or 2),
 * both strings and integers are valid values. The full list of parameters is defined at:
 *   https://developers.google.com/youtube/player_parameters?playerVersion=HTML5.
 *
 * This method reloads the entire contents of the webview and regenerates its HTML contents.
 * To change the currently loaded video without reloading the entire webview, use the
 * YTPlayerView::cuePlaylistByPlaylistId:index:startSeconds:
 * family of methods.
 *
 * @param playlistId The YouTube playlist ID of the playlist to load in the player view.
 * @param playerVars An NSDictionary of player parameters.
 * @return YES if player has been configured correctly, NO otherwise.
 */
- (BOOL)loadWithPlaylistId:(nonnull NSString *)playlistId
                playerVars:(nullable NSDictionary *)playerVars;

/**
 * This method loads an iframe player with the given player parameters. Usually you may want to use
 * -loadWithVideoId:playerVars: or -loadWithPlaylistId:playerVars: instead of this method does not handle
 * video_id or playlist_id at all. The full list of parameters is defined at:
 *   https://developers.google.com/youtube/player_parameters?playerVersion=HTML5.
 *
 * @param additionalPlayerParams An NSDictionary of parameters in addition to required parameters
 *                               to instantiate the HTML5 player with. This differs depending on
 *                               whether a single video or playlist is being loaded.
 * @return YES if successful, NO if not.
 */
- (BOOL)loadWithPlayerParams:(nullable NSDictionary *)additionalPlayerParams;

#pragma mark - Player controls

// These methods correspond to their JavaScript equivalents as documented here:
//   https://developers.google.com/youtube/iframe_api_reference#Playback_controls

/**
 * Starts or resumes playback on the loaded video. Corresponds to this method from
 * the JavaScript API:
 *   https://developers.google.com/youtube/iframe_api_reference#playVideo
 */
- (void)playVideo;

/**
 * Pauses playback on a playing video. Corresponds to this method from
 * the JavaScript API:
 *   https://developers.google.com/youtube/iframe_api_reference#pauseVideo
 */
- (void)pauseVideo;

/**
 * Stops playback on a playing video. Corresponds to this method from
 * the JavaScript API:
 *   https://developers.google.com/youtube/iframe_api_reference#stopVideo
 */
- (void)stopVideo;

/**
 * Seek to a given time on a playing video. Corresponds to this method from
 * the JavaScript API:
 *   https://developers.google.com/youtube/iframe_api_reference#seekTo
 *
 * @param seekToSeconds The time in seconds to seek to in the loaded video.
 * @param allowSeekAhead Whether to make a new request to the server if the time is
 *                       outside what is currently buffered. Recommended to set to YES.
 */
- (void)seekToSeconds:(float)seekToSeconds allowSeekAhead:(BOOL)allowSeekAhead;

#pragma mark - Cueing videos

// Cueing functions for videos. These methods correspond to their JavaScript
// equivalents as documented here:
//   https://developers.google.com/youtube/iframe_api_reference#Queueing_Functions

/**
 * Cues a given video by its video ID for playback starting at the given time.
 * Cueing loads a video, but does not start video playback. This method
 * corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#cueVideoById
 *
 * @param videoId A video ID to cue.
 * @param startSeconds Time in seconds to start the video when YTPlayerView::playVideo is called.
 */
- (void)cueVideoById:(nonnull NSString *)videoId
        startSeconds:(float)startSeconds;

/**
 * Cues a given video by its video ID for playback starting and ending at the given times.
 * Cueing loads a video, but does not start video playback. This
 * method corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#cueVideoById
 *
 * @param videoId A video ID to cue.
 * @param startSeconds Time in seconds to start the video when playVideo() is called.
 * @param endSeconds Time in seconds to end the video after it begins playing.
 */
- (void)cueVideoById:(nonnull NSString *)videoId
        startSeconds:(float)startSeconds
          endSeconds:(float)endSeconds;

/**
 * Loads a given video by its video ID for playback starting at the given time.
 * Loading a video both loads it and begins playback. This method
 * corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#loadVideoById
 *
 * @param videoId A video ID to load and begin playing.
 * @param startSeconds Time in seconds to start the video when it has loaded.
 */
- (void)loadVideoById:(nonnull NSString *)videoId
         startSeconds:(float)startSeconds;

/**
 * Loads a given video by its video ID for playback starting and ending at the given times.
 * Loading a video both loads it and begins playback. This method
 * corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#loadVideoById
 *
 * @param videoId A video ID to load and begin playing.
 * @param startSeconds Time in seconds to start the video when it has loaded.
 * @param endSeconds Time in seconds to end the video after it begins playing.
 */
- (void)loadVideoById:(nonnull NSString *)videoId
         startSeconds:(float)startSeconds
           endSeconds:(float)endSeconds;

/**
 * Cues a given video by its URL on YouTube.com for playback starting at the given time.
 * Cueing loads a video, but does not start video playback.
 * This method corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#cueVideoByUrl
 *
 * @param videoURL URL of a YouTube video to cue for playback.
 * @param startSeconds Time in seconds to start the video when YTPlayerView::playVideo is called.
 */
- (void)cueVideoByURL:(nonnull NSString *)videoURL
         startSeconds:(float)startSeconds;

/**
 * Cues a given video by its URL on YouTube.com for playback starting at the given time.
 * Cueing loads a video, but does not start video playback.
 * This method corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#cueVideoByUrl
 *
 * @param videoURL URL of a YouTube video to cue for playback.
 * @param startSeconds Time in seconds to start the video when YTPlayerView::playVideo is called.
 * @param endSeconds Time in seconds to end the video after it begins playing.
 */
- (void)cueVideoByURL:(nonnull NSString *)videoURL
         startSeconds:(float)startSeconds
           endSeconds:(float)endSeconds;

/**
 * Loads a given video by its video ID for playback starting at the given time.
 * Loading a video both loads it and begins playback. This method
 * corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#loadVideoByUrl
 *
 * @param videoURL URL of a YouTube video to load and play.
 * @param startSeconds Time in seconds to start the video when it has loaded.
 */
- (void)loadVideoByURL:(nonnull NSString *)videoURL
          startSeconds:(float)startSeconds;

/**
 * Loads a given video by its video ID for playback starting and ending at the given times.
 * Loading a video both loads it and begins playback. This method
 * corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#loadVideoByUrl
 *
 * @param videoURL URL of a YouTube video to load and play.
 * @param startSeconds Time in seconds to start the video when it has loaded.
 * @param endSeconds Time in seconds to end the video after it begins playing.
 */
- (void)loadVideoByURL:(nonnull NSString *)videoURL
          startSeconds:(float)startSeconds
            endSeconds:(float)endSeconds;

#pragma mark - Cueing functions for playlists

// Cueing functions for playlists. These methods correspond to
// the JavaScript methods defined here:
//    https://developers.google.com/youtube/js_api_reference#Playlist_Queueing_Functions

/**
 * Cues a given playlist with the given ID. The |index| parameter specifies the 0-indexed
 * position of the first video to play, starting at the given time. Cueing loads a playlist,
 * but does not start video playback. This method corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#cuePlaylist
 *
 * @param playlistId Playlist ID of a YouTube playlist to cue.
 * @param index A 0-indexed position specifying the first video to play.
 * @param startSeconds Time in seconds to start the video when YTPlayerView::playVideo is called.
 */
- (void)cuePlaylistByPlaylistId:(nonnull NSString *)playlistId
                          index:(int)index
                   startSeconds:(float)startSeconds;

/**
 * Cues a playlist of videos with the given video IDs. The |index| parameter specifies the
 * 0-indexed position of the first video to play, starting at the given time.
 * Cueing loads a playlist, but does not start video playback. This method
 * corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#cuePlaylist
 *
 * @param videoIds An NSArray of video IDs to compose the playlist of.
 * @param index A 0-indexed position specifying the first video to play.
 * @param startSeconds Time in seconds to start the video when YTPlayerView::playVideo is called.
 */
- (void)cuePlaylistByVideos:(nonnull NSArray *)videoIds
                      index:(int)index
               startSeconds:(float)startSeconds;

/**
 * Loads a given playlist with the given ID. The |index| parameter specifies the 0-indexed
 * position of the first video to play, starting at the given time. Loading a playlist starts video playback. This method
 * corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#loadPlaylist
 *
 * @param playlistId Playlist ID of a YouTube playlist to cue.
 * @param index A 0-indexed position specifying the first video to play.
 * @param startSeconds Time in seconds to start the video when YTPlayerView::playVideo is called.
 */
- (void)loadPlaylistByPlaylistId:(nonnull NSString *)playlistId
                           index:(int)index
                    startSeconds:(float)startSeconds;

/**
 * Loads a playlist of videos with the given video IDs. The |index| parameter specifies the
 * 0-indexed position of the first video to play, starting at the given time. Loading a playlist starts video playback.
 * This method corresponds with its JavaScript API equivalent as documented here:
 *    https://developers.google.com/youtube/iframe_api_reference#loadPlaylist
 *
 * @param videoIds An NSArray of video IDs to compose the playlist of.
 * @param index A 0-indexed position specifying the first video to play.
 * @param startSeconds Time in seconds to start the video when YTPlayerView::playVideo is called.
 */
- (void)loadPlaylistByVideos:(nonnull NSArray *)videoIds
                       index:(int)index
                startSeconds:(float)startSeconds;

#pragma mark - Playing a video in a playlist

// These methods correspond to the JavaScript API as defined under the
// "Playing a video in a playlist" section here:
//    https://developers.google.com/youtube/iframe_api_reference#Playback_status

/**
 * Loads and plays the next video in the playlist. Corresponds to this method from
 * the JavaScript API:
 *   https://developers.google.com/youtube/iframe_api_reference#nextVideo
 */
- (void)nextVideo;

/**
 * Loads and plays the previous video in the playlist. Corresponds to this method from
 * the JavaScript API:
 *   https://developers.google.com/youtube/iframe_api_reference#previousVideo
 */
- (void)previousVideo;

/**
 * Loads and plays the video at the given 0-indexed position in the playlist.
 * Corresponds to this method from the JavaScript API:
 *   https://developers.google.com/youtube/iframe_api_reference#playVideoAt
 *
 * @param index The 0-indexed position of the video in the playlist to load and play.
 */
- (void)playVideoAt:(int)index;

#pragma mark - Setting the playback rate

/**
 * Gets the playback rate. The default value is 1.0, which represents a video
 * playing at normal speed. Other values may include 0.25 or 0.5 for slower
 * speeds, and 1.5 or 2.0 for faster speeds. This method corresponds to the
 * JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getPlaybackRate
 * @param completionHandler async callback block that contains a float value representing the current value
 * or an error.
 */
- (void)playbackRate:(_Nullable YTFloatCompletionHandler)completionHandler;

/**
 * Sets the playback rate. The default value is 1.0, which represents a video
 * playing at normal speed. Other values may include 0.25 or 0.5 for slower
 * speeds, and 1.5 or 2.0 for faster speeds. To fetch a list of valid values for
 * this method, call YTPlayerView::getAvailablePlaybackRates. This method does not
 * guarantee that the playback rate will change.
 * This method corresponds to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#setPlaybackRate
 *
 * @param suggestedRate A playback rate to suggest for the player.
 */
- (void)setPlaybackRate:(float)suggestedRate;

/**
 * Gets a list of the valid playback rates, useful in conjunction with
 * YTPlayerView::setPlaybackRate. This method corresponds to the
 * JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getPlaybackRate
 *
 * @param completionHandler async callback block that contains an array containing the available
 * playback rates or an error.
 */
- (void)availablePlaybackRates:(_Nullable YTArrayCompletionHandler)completionHandler;

#pragma mark - Setting playback behavior for playlists

/**
 * Sets whether the player should loop back to the first video in the playlist
 * after it has finished playing the last video. This method corresponds to the
 * JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#loopPlaylist
 *
 * @param loop A boolean representing whether the player should loop.
 */
- (void)setLoop:(BOOL)loop;

/**
 * Sets whether the player should shuffle through the playlist. This method
 * corresponds to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#shufflePlaylist
 *
 * @param shuffle A boolean representing whether the player should
 *                shuffle through the playlist.
 */
- (void)setShuffle:(BOOL)shuffle;

#pragma mark - Playback status
// These methods correspond to the JavaScript methods defined here:
//    https://developers.google.com/youtube/js_api_reference#Playback_status

/**
 * Returns a number between 0 and 1 that specifies the percentage of the video
 * that the player shows as buffered. This method corresponds to the
 * JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getVideoLoadedFraction
 *
 * @param completionHandler async callback block that contains a float number with the result or an error.
 */
- (void)videoLoadedFraction:(_Nullable YTFloatCompletionHandler)completionHandler;

/**
 * Returns the state of the player. This method corresponds to the
 * JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getPlayerState
 *
 * @param completionHandler async callback block that contains a YTPlayerState enum value
 * with the current player state or an error.
 */
- (void)playerState:(_Nullable YTPlayerStateCompletionHandler)completionHandler;

/**
 * Returns the elapsed time in seconds since the video started playing. This
 * method corresponds to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getCurrentTime
 *
 * @param completionHandler async callback block that contains float number with the result or an error.
 */
- (void)currentTime:(_Nullable YTFloatCompletionHandler)completionHandler;

#pragma mark - Retrieving video information

// Retrieving video information. These methods correspond to the JavaScript
// methods defined here:
//   https://developers.google.com/youtube/js_api_reference#Retrieving_video_information

/**
 * Returns the duration in seconds since the video of the video. This
 * method corresponds to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getDuration
 *
 * @param completionHandler async callback block that contains a double number
 * with the duration of the video or an error.
 */
- (void)duration:(_Nullable YTDoubleCompletionHandler)completionHandler;

/**
 * Returns the YouTube.com URL for the video. This method corresponds
 * to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getVideoUrl
 *
 * @param completionHandler async callback block that contains the video URL as NSURL or an error.
 */
- (void)videoUrl:(_Nullable YTURLCompletionHandler)completionHandler;

/**
 * Returns the embed code for the current video. This method corresponds
 * to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getVideoEmbedCode
 *
 * @param completionHandler async callback block that contains a string with the current video
 * embed code or an error.
 */
- (void)videoEmbedCode:(_Nullable YTStringCompletionHandler)completionHandler;

#pragma mark - Retrieving playlist information

// Retrieving playlist information. These methods correspond to the
// JavaScript defined here:
//    https://developers.google.com/youtube/js_api_reference#Retrieving_playlist_information

/**
 * Returns an ordered array of video IDs in the playlist. This method corresponds
 * to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getPlaylist
 *
 * @param completionHandler async callback block that contains an array of video IDs in the playlist
 * or an error.
 */
- (void)playlist:(_Nullable YTArrayCompletionHandler)completionHandler;

/**
 * Returns the 0-based index of the currently playing item in the playlist.
 * This method corresponds to the JavaScript API defined here:
 *   https://developers.google.com/youtube/iframe_api_reference#getPlaylistIndex
 *
 * @param completionHandler async callback block that contains the int value of the current playing item
 * in a playlist or an error.
 */
- (void)playlistIndex:(_Nullable YTIntCompletionHandler)completionHandler;

#pragma mark - Exposed for Testing

/**
 * Removes the internal web view from this player view.
 * Intended to use for testing, should not be used in production code.
 */
- (void)removeWebView;

@end
