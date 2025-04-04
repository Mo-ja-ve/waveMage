#include <iostream>
#include <stdio.h>
#ifndef IMGUI_H
#define IMGUI_H
#include "imgui.h"
#endif  
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"
#include "implot.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
//#include "raudio.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

const float PI = 3.141593;

typedef struct {//  our sound file has a right and left channel (Stereophonic sound)
	float left;
	float right;
} stereo_frame;

std::vector<int> v;

std::vector<stereo_frame> vecFrames;

// 3340
// line 
// 13. Audio Buffers
// =================
// miniaudio supports reading from a buffer of raw audio data via the `ma_audio_buffer` API. This can
// read from memory that's managed by the application, but can also handle the memory management for
// you internally. Memory management is flexible and should support most use cases.


void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    // We're only interested in capturing microphone input, so we can ignore pOutput and pInput.
    //MA_ASSERT(pDevice->capture.pInterleavedSamples != NULL);
    //MA_ASSERT(pOutput == NULL);
    // In a real-world scenario, you would process or store the captured audio data here.   
    // pInput contains the captured audio data.
    ma_encoder_write_pcm_frames((ma_encoder*)pDevice->pUserData, pInput, frameCount, NULL);
    
    //std::cout<<endl<<"1 frameCount: "<<frameCount;
    
    (void)pOutput;
}

void duplex_dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    
    //std::cout<<std::endl<<"2 frameCount: "<<frameCount;
    //stereo_frame* tempBuff = (stereo_frame *)pInput;
    
    // if(vecFrames.size() < vecFrames.capacity()){
        //     copy(&tempBuff[0], &tempBuff[frameCount], back_inserter(vecFrames));
        // }else{
            //     for(int i = 0; i < vecFrames.size(); i++){   
                //         std::cout<<std::endl<<"vecFrames: "<<vecFrames[i].left;
                //         std::cout<<std::endl<<"vecFrames: "<<vecFrames[i].right;
                //     }
                // } 
                //stereo_frame* tempBuff = (stereo_frame *)pInput;

    
        /* This example assumes the playback and capture sides use the same format and channel count. */
    if (pDevice->capture.format != pDevice->playback.format || pDevice->capture.channels != pDevice->playback.channels) {
        return;
    }

    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    
    if(vecFrames.size() + 441 < vecFrames.capacity()){
        //copy(&tempBuff[0], &tempBuff[frameCount], back_inserter(vecFrames));
                                                        // this might not give you the right address 
        copy(&(*(stereo_frame*)pInput), &(*(stereo_frame*)(pInput + frameCount)), back_inserter(vecFrames));
    }else{
        // for(int i = 0; i < vecFrames.size(); i++){ 
        //     std::cout<<std::endl<<"vecFrames: "<<vecFrames[i].left;
        //     std::cout<<std::endl<<"vecFrames: "<<vecFrames[i].right;
        // }
    }
                
}
            
            
int main(int argc, char* argv[]) {
                
    ma_result result;
    ma_device device, duplexDevice;
    ma_context context; 
    
    ma_device_info* pPlaybackInfos;
    ma_uint32 playbackCount;
    ma_device_info* pCaptureInfos;
    ma_uint32 captureCount;
    
    ma_encoder_config encoderConfig;
    ma_encoder encoder;
    ma_audio_buffer* pBuffer;
    
    vecFrames.reserve(4800);

    ma_audio_buffer_config config = ma_audio_buffer_config_init( ma_format_f32, 2, vecFrames.capacity(), NULL, NULL);

    result = ma_audio_buffer_alloc_and_init(&config, &pBuffer);
    if (result != MA_SUCCESS) {
        // Error
    }

    encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32, 2, 44100);
    
    //char filename[] = "test.wav";
    result = ma_encoder_init_file(argv[1], &encoderConfig, &encoder);
    if( result != MA_SUCCESS){
        std::cerr << "ERROR: " << ma_result_description(result) << " (Code: " << result << ")" << std::endl;
    }

    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        std::cout<<std::endl<<"error! failed to init ma context!";
    }
    if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS) {
        std::cout<<std::endl<<"error! failed to enumerate devices!";
    }
    // Loop over each device info and do something with it. Here we just print the name with their index. You may want
    // to give the user the opportunity to choose which device they'd prefer.
    for (ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
        printf("%d - %s\n", iDevice, pPlaybackInfos[iDevice].name);
    }
    
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = encoder.config.format;
    deviceConfig.capture.channels = encoder.config.channels;
    deviceConfig.sampleRate       = encoder.config.sampleRate;
    deviceConfig.dataCallback     = dataCallback;
    deviceConfig.pUserData        = &encoder;
    
    // for (ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
    //     printf("%d - %s\n", iDevice, pPlaybackInfos[iDevice].name);
    // }

    ma_device_config duplex_deviceConfig = ma_device_config_init(ma_device_type_duplex);
    duplex_deviceConfig.playback.pDeviceID = NULL;
    duplex_deviceConfig.capture.format   = ma_format_f32;
    duplex_deviceConfig.capture.channels = 2;
    duplex_deviceConfig.playback.format   = ma_format_f32;
    duplex_deviceConfig.playback.channels = 2; 
    duplex_deviceConfig.sampleRate       = 44100;
    duplex_deviceConfig.dataCallback     = duplex_dataCallback;
    //duplex_deviceConfig.pUserData        = &pBuffer;
    
    std::cout<<std::endl<<"deviceConfig.capture.pDeviceID: "<<deviceConfig.capture.pDeviceID<<std::endl;
    std::cout<<std::endl<<"duplex_deviceConfig.capture.pDeviceID: "<<duplex_deviceConfig.capture.pDeviceID<<std::endl;
    std::cout<<std::endl<<"duplex_deviceConfig.playback.pDeviceID: "<<duplex_deviceConfig.playback.pDeviceID<<std::endl;
    
    

    result = ma_device_init(NULL, &deviceConfig, &device);
    if(result != MA_SUCCESS){
        std::cerr << "Error: " << ma_result_description(result) << " (Code: " << result << ")" << std::endl;
    }
    result = ma_device_init(NULL, &duplex_deviceConfig, &duplexDevice);
    if(result != MA_SUCCESS){
        std::cerr << "Error: " << ma_result_description(result) << " (Code: " << result << ")" << std::endl;
    }
    
    // ma_device_start(&device);
    ma_device_start(&duplexDevice);

    std::cout<<std::endl<<"context.backend: "<<context.backend;

    printf("Press Enter to stop recording...\n");
    getchar();
    
    // ma_device_uninit(&device);
    // ma_encoder_uninit(&encoder);

    ma_device_uninit(&duplexDevice);
    ma_audio_buffer_uninit(pBuffer);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    ImPlotPoint SineWave(int idx, void* wave_data);

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif  

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;


} 
