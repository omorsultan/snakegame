void Snake::spawnFood()
{
    // Random number generator setup
    static std::random_device rd; // Random device for seed
    static std::mt19937 gen(rd()); // Mersenne Twister RNG
    static std::uniform_int_distribution<> disX(0, (SCREEN_WIDTH / TILE_SIZE) - 1); // X coordinate range
    static std::uniform_int_distribution<> disY(0, ((SCREEN_HEIGHT - TILE_SIZE * 4) / TILE_SIZE) - 1); // Y coordinate range

    // Define wall boundaries for collision checks
    SDL_Rect wall1 = {360, 240, 20, 240}; // Left wall
    SDL_Rect wall2 = {720, 240, 20, 240}; // Right wall
    SDL_Rect wall3 = {480, 240, 120, 20}; // Top wall
    SDL_Rect wall4 = {480, 470, 120, 20}; // Bottom wall
    SDL_Rect wall6 = {0, 55, SCREEN_WIDTH, TILE_SIZE / 4}; // Top boundary

    SDL_Point foodPosition;

    do
    {
        // Randomly generate a position for the food
        foodPosition.x = disX(gen) * TILE_SIZE;
        foodPosition.y = disY(gen) * TILE_SIZE;
    } while (
        // Check if food spawns inside a wall
        (foodPosition.x >= wall1.x && foodPosition.x < wall1.x + wall1.w && foodPosition.y >= wall1.y && foodPosition.y < wall1.y + wall1.h) ||
        (foodPosition.x >= wall2.x && foodPosition.x < wall2.x + wall2.w && foodPosition.y >= wall2.y && foodPosition.y < wall2.y + wall2.h) ||
        (foodPosition.x >= wall3.x && foodPosition.x < wall3.x + wall3.w && foodPosition.y >= wall3.y && foodPosition.y < wall3.y + wall3.h) ||
        (foodPosition.x >= wall4.x && foodPosition.x < wall4.x + wall4.w && foodPosition.y >= wall4.y && foodPosition.y < wall4.y + wall4.h) ||
        // Check if food spawns at the top boundary
        (foodPosition.x >= 0 && foodPosition.x < SCREEN_WIDTH && foodPosition.y >= 0 && foodPosition.y < 60) ||
        // Check if food overlaps with the snake's body
        std::any_of(body.begin(), body.end(), [foodPosition](const SDL_Rect &segment)
                    { return foodPosition.x == segment.x && foodPosition.y == segment.y; }) ||
        // Ensure food doesn't spawn in recently used positions
        std::find(recentPositions.begin(), recentPositions.end(), foodPosition) != recentPositions.end()
    );

    // Set the food position and size
    food.x = foodPosition.x;
    food.y = foodPosition.y;
    food.w = TILE_SIZE;
    food.h = TILE_SIZE;

    // Store the food position to avoid repeating locations
    recentPositions.push_back(foodPosition);
    if (recentPositions.size() > 5) // Keep only the last 5 positions
        recentPositions.erase(recentPositions.begin());

    // Handle bonus food spawn every 7th score
    if (score % 7 == 0)
    {
        bonusFoodActive = true; // Activate bonus food
        bonusFoodTimer = SDL_GetTicks() + 7000; // Bonus food duration: 7 seconds

        SDL_Point bonusFoodPosition;

        do
        {
            // Generate random position for bonus food
            bonusFoodPosition.x = disX(gen) * TILE_SIZE;
            bonusFoodPosition.y = disY(gen) * TILE_SIZE;
        } while (
            // Check collision with walls
            (bonusFoodPosition.x >= wall1.x && bonusFoodPosition.x < wall1.x + wall1.w && bonusFoodPosition.y >= wall1.y && bonusFoodPosition.y < wall1.y + wall1.h) ||
            (bonusFoodPosition.x >= wall2.x && bonusFoodPosition.x < wall2.x + wall2.w && bonusFoodPosition.y >= wall2.y && bonusFoodPosition.y < wall2.y + wall2.h) ||
            (bonusFoodPosition.x >= wall3.x && bonusFoodPosition.x < wall3.x + wall3.w && bonusFoodPosition.y >= wall3.y && bonusFoodPosition.y < wall3.y + wall3.h) ||
            (bonusFoodPosition.x >= wall4.x && bonusFoodPosition.x < wall4.x + wall4.w && bonusFoodPosition.y >= wall4.y && bonusFoodPosition.y < wall4.y + wall4.h) ||
            // Check top boundary
            (bonusFoodPosition.x >= 0 && bonusFoodPosition.x < SCREEN_WIDTH && bonusFoodPosition.y >= 0 && bonusFoodPosition.y < 60) ||
            // Check if it overlaps with the snake's body
            std::any_of(body.begin(), body.end(), [bonusFoodPosition](const SDL_Rect &segment)
                        { return bonusFoodPosition.x == segment.x && bonusFoodPosition.y == segment.y; })
        );

        // Set bonus food position and size
        bonusFood.x = bonusFoodPosition.x;
        bonusFood.y = bonusFoodPosition.y;
        bonusFood.w = TILE_SIZE * 2;
        bonusFood.h = TILE_SIZE * 2;
    }
}
