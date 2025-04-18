Pod::Spec.new do |s|
  s.name              = "youtube-ios-player-helper"
  s.version           = "1.0.5"
  s.summary           = "Helper library for iOS developers that want to embed YouTube videos in
                         their iOS apps with the iframe player API."

  s.description       = <<-DESC
                       Helper library for iOS developers that want to play YouTube videos in
                       their iOS apps with the iframe player API.

                       This library allows iOS developers to quickly embed YouTube videos within
                       their applications via a custom UIView subclass, YTPlayerView.
                       This library provides:

                       * A managed WebView instance that loads the HTML code for the iframe player
                       * Objective-C wrapper functions for the JavaScript Player API
                       * YTPlayerViewDelegate for handling YouTube player state changes natively in
                         your Objective-C code
                       DESC

  s.homepage           = "https://developers.google.com/youtube/v3/guides/ios_youtube_helper"
  s.license            = {
                            :type => 'Apache',
                            :text => <<-LICENSE
                              Copyright 2014 Google Inc. All rights reserved.

                              Licensed under the Apache License, Version 2.0 (the "License");
                              you may not use this file except in compliance with the License.
                              You may obtain a copy of the License at

                              http://www.apache.org/licenses/LICENSE-2.0

                              Unless required by applicable law or agreed to in writing, software
                              distributed under the License is distributed on an "AS IS" BASIS,
                              WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
                              See the License for the specific language governing permissions and
                              limitations under the License.
                            LICENSE
                         }
  s.author             = { "Ikai Lan" => "",
                           "Ibrahim Ulukaya" => "",
                           "Yoshifumi Yamaguchi" => "" }
  s.social_media_url   = "https://twitter.com/YouTubeDev"
  s.source             = { :git => "https://github.com/youtube/youtube-ios-player-helper.git", :tag => "1.0.5" }

  s.platform     = :ios, '10.0'
  s.requires_arc = true

  s.source_files = "Sources/**/*.{h,m}"
  
  s.resource_bundle = {
    'Assets' => ['Sources/Assets/*.html']
  }

  s.ios.exclude_files = 'Sources/osx'
  s.osx.exclude_files = 'Sources/ios'
end
