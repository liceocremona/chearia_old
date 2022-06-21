from fastapi import FastAPI, Depends
from fastapi.responses import RedirectResponse
from fastapi.middleware.cors import CORSMiddleware
from routers import board, resources

# Creating a FastAPI app and adding CORS middleware to it.
app = FastAPI()

origins=["https://progettochearia.it",
 "https://web.progettochearia.it",
 "https://admin.progettochearia.it"]
app.add_middleware(
    CORSMiddleware,
    allow_origins= origins,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Including the routers from the routers.py file.
app.include_router(
    board.router,
    prefix="/board",
)
app.include_router(
    resources.router,
    prefix="/resources",
)

"""
    It redirects the user to the website https://progettochearia.it
    :return: A redirect to the website.
"""
@app.get("/", response_class=RedirectResponse, status_code=302)
async def root():
    return "https://progettochearia.it"

