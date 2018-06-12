Rails.application.routes.draw do
  resources :trainers do
    resources :bookings do
      resources :reviews, only: [:new, :create]
    end
    resources :reviews, only: [:index, :show]
  end
  resources :about, only: [:index]
  resources :users

  # get 'expertises/index'
  # get 'expertises/show'
  # get 'expertises/new'
  # get 'expertises/create'
  # get 'expertises/edit'
  # get 'expertises/update'
  # get 'expertises/destroy'
  # get 'users/index'
  # get 'users/show'
  # get 'users/new'
  # get 'users/create'
  # get 'users/edit'
  # get 'users/update'
  # get 'users/destroy'
  # get 'bookings/index'
  # get 'bookings/show'
  # get 'bookings/new'
  # get 'bookings/create'
  # get 'bookings/edit'
  # get 'bookings/update'
  # get 'bookings/destroy'
  # get 'trainers/index'
  # get 'trainers/show'
  # get 'trainers/new'
  # get 'trainers/create'
  # get 'trainers/edit'
  # get 'trainers/update'
  # get 'trainers/destroy'
  # get 'reviews/create'
  # get 'reviews/new'
  # get 'reviews/show'
  # get 'reviews/index'
  devise_for :users
  root to: 'pages#home'
  # For details on the DSL available within this file, see http://guides.rubyonrails.org/routing.html
end
