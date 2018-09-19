from django.shortcuts import render

def index(request):
    return render(request, 'personal/header.html')
