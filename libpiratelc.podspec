Pod::Spec.new do |s|
    s.name             = 'libpiratelc'
    s.version          = '0.0.5'
    s.summary          = 'Rust core for Pirate clients'
    s.homepage         = 'https://github.com/piratenetwork/pirate-light-client-ffi'
    s.license          = { :type => 'MIT', :file => 'LICENSE' }
    s.author           = {
        'Francisco Gindre' => 'francisco.gindre@gmail.com',
        'Jack Grigg' => 'str4d@electriccoin.co',
        'Forge' => 'cryptoforge.cc@protonmail.com'
     }
    s.source           = { :git => 'https://github.com/piratenetwork/pirate-light-client-ffi.git', :tag => s.version.to_s }
    s.vendored_frameworks = 'releases/XCFramework/libpiratelc.xcframework'
    s.preserve_paths = 'releases/XCFramework/libpiratelc.xcframework'
    s.ios.deployment_target = '12.0'
    s.static_framework = true
end
